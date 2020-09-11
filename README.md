*(All requirements below are satisfied.)*

## 实验1：Linux进程管理及其扩展（综合型，必做）

### 实验目的

通过实验，加深理解进程控制块、进程队列等概念，了解进程管理的具体实施方法。

### 实验内容

- 阅读并分析Linux内核源代码，了解进程控制块、进程队列等数据结构；
- 实现一个系统调用，使得可以根据指定的参数隐藏进程，使用户无法使用ps或top观察到进程状态。具体要求如下：
  1. 实现系统调用int hide(pid_t pid, int on)，在进程pid有效的前提下，如果on置1，进程被隐藏，用户无法通过ps或top观察到进程状态；如果on置0且此前为隐藏状态，则恢复正常状态。
  2. 考虑权限问题，只有根用户才能隐藏进程。
  3. 设计一个新的系统调用int hide_user_processes(uid_t uid, char *binname)，参数uid为用户ID号，当binname参数为NULL时，隐藏该用户的所有进程；否则，隐藏二进制映像名为binname的用户进程。该系统调用应与hide系统调用共存。
  4. 在/proc目录下创建一个文件/proc/hidden，该文件可读可写，对应一个全局变量hidden_flag，当hidden_flag为0时，所有进程都无法隐藏，即便此前进程被hide系统调用要求隐藏。只有当hidden_flag为1时，此前通过hide调用要求被屏蔽的进程才隐藏起来。（选做）
  5. 在/proc目录下创建一个文件/proc/hidden_process，该文件的内容包含所有被隐藏进程的pid，各pid之间用空格分开。（选做）