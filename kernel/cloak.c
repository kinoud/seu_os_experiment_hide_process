#include <linux/syscalls.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/proc_fs.h>
#include <linux/seq_file.h>
#include<linux/sched.h>
#include<linux/cloak.h>

static struct proc_dir_entry *hidden,*hidden_process;

static char kbuf[5];
int hidden_flag;

static void *hidden_seq_start(struct seq_file *s, loff_t *pos){
    struct task_struct *p=&init_task;
    int i=*pos;
    //printk("[hide] pos = %lld\n",*pos);
    while(i--){
        p=next_task(p);
        if(p==&init_task)return NULL;
    }
    return p;
}

static void *hidden_seq_next(struct seq_file *s,void *v,loff_t *pos){
    struct task_struct *p=v;
    if(next_task(p)==&init_task)return NULL;
    ++(*pos);
    return next_task(p);
}

static void hidden_seq_stop(struct seq_file *s,void *v){
}

static int hidden_seq_show(struct seq_file *s,void *v){
    struct task_struct *p=v;
    if(p->cloak==1)
        seq_printf(s,"%d ",(int)task_pid_vnr(p));
    return 0;
}

static const struct seq_operations hidden_seq_ops = {
    .start = hidden_seq_start,
    .next = hidden_seq_next,
    .stop = hidden_seq_stop,
    .show = hidden_seq_show
};

static int hidden_single_show(struct seq_file *s,void *v){
    seq_printf(s,"%d",hidden_flag);
    return 0;
}

static int hidden_single_open(struct inode *inode, struct file *file)
{
	return single_open(file, hidden_single_show, NULL);
}

static ssize_t  hidden_single_write(struct file *file, const char __user *buf, size_t count, loff_t *pos){
    int len=1;
    if(copy_from_user(kbuf,buf,len))
        return -EFAULT;
    if(kbuf[0]=='0')
        hidden_flag=0;
    else
        hidden_flag=1;
    return len;
}

static const struct proc_ops hidden_single_ops = {
	/* not permanent -- can call into arbitrary ->single_show */
	.proc_open	= hidden_single_open,
	.proc_read	= seq_read,
    .proc_write = hidden_single_write,
	.proc_lseek	= seq_lseek,
	.proc_release	= single_release,
};


static int __init proc_hidden_init(void){
    hidden_flag=0;
    hidden = proc_create("hidden",0600,NULL,&hidden_single_ops);
    hidden_process = proc_create_seq("hidden_process", 0400, NULL, &hidden_seq_ops);
    return 0;
}

subsys_initcall(proc_hidden_init);

SYSCALL_DEFINE2(hide_proc,pid_t,vpid,int,on){
	struct task_struct *p;
	struct pid *thread_pid;

	printk("hide_proc invoked with params: pid=%d on=%d\n",vpid,on);
	if (!uid_eq(current_euid(),GLOBAL_ROOT_UID)){
		printk("you dont have permission!\n");
		return -1;
	}
	
	p = find_task_by_vpid(vpid);
	if(on==1)
		p -> cloak = 1;
	else if(on==0)
		p -> cloak = 0;
	else{
		printk("invalid param: on=%d\n",on);
		return -2;
	}
	
	thread_pid = get_pid(p->thread_pid);
	proc_flush_pid(thread_pid);
		
	return 0;
}

SYSCALL_DEFINE2(hide_user_proc,uid_t,uid,char*,binname){

	struct task_struct *p;
	struct user_namespace *ns = current_user_ns();
	struct pid *thread_pid;
	char buf[TASK_COMM_LEN];
	char buf2[TASK_COMM_LEN];
	int hide,i;
	kuid_t kuid;

	kuid = make_kuid(ns, uid);
	

	printk("hide_user_proc invoked");
	
	printk(" uid=%d ",uid);
	if(binname!=NULL){
		copy_from_user(buf2,binname,TASK_COMM_LEN);
		printk(" binname=%s",buf2);
	}else printk(" binname=NULL");
	
	
	for_each_process(p){
		if(uid_eq(task_uid(p),kuid)){
			hide=1;
			get_task_comm(buf, p);
			
			if(binname!=NULL){
				for(i=0;buf2[i];i++){
					if(!buf[i]||buf[i]!=buf2[i]){
						hide=0;
						break;
					}
				}
				if(buf[i])hide=0;
			}
			printk("scan on '%s' hide it? =%d",buf,hide);

			if(hide){
				p -> cloak = 1;
				thread_pid = get_pid(p->thread_pid);
				proc_flush_pid(thread_pid);
			}
		}
	}

	return 0;
}