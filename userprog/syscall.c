#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "lib/user/syscall.h"
#include "include/filesys/filesys.h"
#include "include/filesys/file.h"
#include "userprog/process.h"
#include "lib/string.h"
#include "threads/palloc.h"

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

int allocate_fd(struct file *file);
struct file *get_file_by_fd(int fd);
void check_address(void*);
pid_t sys_fork (const char *thread_name, struct intr_frame *f);

/* 시스템 호출.
 *
 * 이전에는 시스템 호출 서비스가 인터럽트 핸들러에서 처리되었습니다
 * (예: 리눅스에서 int 0x80). 그러나 x86-64에서는 제조사가
 * 시스템 호출을 요청하는 효율적인 경로를 제공합니다. 바로 `syscall` 명령어입니다.
 *
 * syscall 명령어는 모델별 레지스터(MSR)에서 값을 읽어옴으로써 동작합니다.
 * 자세한 내용은 매뉴얼을 참조하세요. */

#define MSR_STAR 0xc0000081         /* 세그먼트 선택자 msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* eflags에 대한 마스크 */
#define FDT_limit 128 /* eflags에 대한 마스크 */

void
syscall_init (void) {
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* 시스템 호출 진입점이 유저 랜드 스택을 커널 모드 스택으로 교체할 때까지
	 * 인터럽트 서비스 루틴은 어떠한 인터럽트도 처리해서는 안됩니다.
	 * 따라서 FLAG_FL을 마스킹합니다. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
    
    /* PDG project2 락 초기화 */
    lock_init(&filesys_lock);
}

/* 주요 시스템 호출 인터페이스 */
void
syscall_handler (struct intr_frame *f UNUSED) {
	// TODO: 여기에 구현을 추가하세요.
	//get stack pointer from interrupt frame
	//get system call number from stack
	uint64_t syscall_number = f->R.rax;
    uint64_t arg1 = f->R.rdi;
    uint64_t arg2 = f->R.rsi;
    uint64_t arg3 = f->R.rdx;
    uint64_t arg4 = f->R.r10;
    uint64_t arg5 = f->R.r8;
    uint64_t arg6 = f->R.r9;

	// if(!(is_user_vaddr(arg1)&&is_user_vaddr(arg2)&&is_user_vaddr(arg3)
	// &&is_user_vaddr(arg4)&&is_user_vaddr(arg5)&&is_user_vaddr(arg6))){
	// 	exit(-1);
    //     return;
	// }


	switch (syscall_number) {
        case SYS_HALT:
            halt();
            break;
        case SYS_EXIT:
            exit((int) arg1);
            break;
        case SYS_FORK:
            f->R.rax = sys_fork(arg1, f);
            break;
        case SYS_EXEC:
            f->R.rax = exec(arg1);
            break;
        case SYS_WAIT:
            f->R.rax = wait((pid_t)arg1);
            break;
        case SYS_CREATE:
            f->R.rax = create((const char *)arg1, (unsigned)arg2);
            break;
        case SYS_REMOVE:
            f->R.rax = remove((const char *)arg1);
            break;
        case SYS_OPEN:
            f->R.rax = open((const char *)arg1);
            break;
        case SYS_FILESIZE:
            f->R.rax = filesize((int)arg1);
            break;
        case SYS_READ:
            f->R.rax = read((int)arg1, (void *)arg2, (unsigned)arg3);
            break;
        case SYS_WRITE:
            f->R.rax = write((int)arg1, (const void *)arg2, (unsigned)arg3);
            break;
        case SYS_SEEK:
            seek((int)arg1, (unsigned)arg2);
            break;
        case SYS_TELL:
            f->R.rax = tell((int)arg1);
            break;
        case SYS_CLOSE:
            close((int)arg1);
            break;
        default:
            exit(-1);
            thread_exit ();
            break;
    }
}
void check_address(void *addr)
{
	if (addr == NULL)
		exit(-1);
	if (!is_user_vaddr(addr))
		exit(-1);
	if (pml4_get_page(thread_current()->pml4, addr) == NULL)
		exit(-1);
}

void halt(void){
	power_off();
}

void exit(int status){
    struct thread *cur = thread_current();
    cur->exit_status = status;
    printf ("%s: exit(%d)\n", cur->name, cur->exit_status);
    thread_exit();
}

pid_t sys_fork (const char *thread_name, struct intr_frame *f){
    pid_t pid = process_fork(thread_name, f);
    return pid;
};

pid_t exec (const char *cmd_line){
    check_address(cmd_line);
    char *fn_copy = palloc_get_page(0);
	if (fn_copy == NULL)
		exit(-1);
    
    strlcpy(fn_copy, cmd_line, PGSIZE);
    pid_t pid = process_exec(fn_copy);

    if(pid==-1){
        exit(-1);
    }
	return pid;
}

int wait(pid_t pid) {
    return process_wait(pid);
}

bool create (const char *file , unsigned initial_size){
    check_address(file);
	lock_acquire(&filesys_lock);
    bool success = filesys_create(file, initial_size);
    lock_release(&filesys_lock);
	return success;
}

bool remove (const char *file){
	lock_acquire(&filesys_lock);
    check_address(file);
	bool success = filesys_remove(file);
    lock_release(&filesys_lock);
    return success;
}


int open(const char *file_name) {
    check_address(file_name);
    lock_acquire(&filesys_lock);
	struct file *file = filesys_open(file_name);
	if (file == NULL){
        lock_release(&filesys_lock);
		return -1;
    }
	int fd = allocate_fd(file);
	if (fd == -1)
		file_close(file);
    lock_release(&filesys_lock);
	return fd;
}

int filesize(int fd) {
    lock_acquire(&filesys_lock);
    struct file *f = get_file_by_fd(fd);
    if (f == NULL) {
	    lock_release(&filesys_lock);
        return -1;
    }
    int answer = file_length(f);
	lock_release(&filesys_lock);
    return answer;
}

int read(int fd, void *buffer, unsigned size) {
    check_address(buffer);
    lock_acquire(&filesys_lock);
    
	int read_result;
	struct thread *cur = thread_current();
	struct file *file_fd = get_file_by_fd(fd);

	if (fd == 0) {
		*(char *)buffer = input_getc();
		read_result = size;
	}
	else {
		if (get_file_by_fd(fd) == NULL) {
            lock_release(&filesys_lock);
			return -1;
		}
		else {
			read_result = file_read(get_file_by_fd(fd), buffer, size);
		}
	}
    lock_release(&filesys_lock);
	return read_result;
}

int write(int fd, const void *buffer, unsigned size) {
	check_address(buffer);
	lock_acquire(&filesys_lock);
    int write_result;
	if (fd == 1) {
		putbuf(buffer, size);
		write_result = size;
	}
	else {
		if (get_file_by_fd(fd) != NULL) {
			write_result = file_write(get_file_by_fd(fd), buffer, size);
		}
		else {
			write_result = -1;
		}
	}
    lock_release(&filesys_lock);
    return write_result;
}

void seek(int fd, unsigned position) {
    lock_acquire(&filesys_lock);
    struct file *f = get_file_by_fd(fd);
    if (f != NULL){
        lock_release(&filesys_lock);
        return;
    }
    file_seek(f, position);
    lock_release(&filesys_lock);
}

unsigned tell(int fd) {
    lock_acquire(&filesys_lock);
    struct file *f = get_file_by_fd(fd);
    if (f == NULL) {
        lock_release(&filesys_lock);
        return;
    }
    int answer = file_tell(f);
    lock_release(&filesys_lock);
    return answer;
}

void close(int fd) {
    struct file *f = get_file_by_fd(fd);
    if (f == NULL) {
        return;
    }
    process_remove_file(fd);
}

int allocate_fd(struct file *file) {
    struct thread *curr = thread_current();
	struct file **fdt = curr->fdt;

	while (curr->next_fd < FDT_limit && fdt[curr->next_fd])
		curr->next_fd++;
	if (curr->next_fd >= FDT_limit)
		return -1;
	fdt[curr->next_fd] = file;

	return curr->next_fd;
}

struct file *get_file_by_fd(int fd) {
    struct thread *t = thread_current();
    if (fd < 2 || fd >= MAX_FILES) {
        return NULL;
    }
    return t->fdt[fd];
}
void process_remove_file(int fd) {
    struct thread *t = thread_current();
    if (fd < 2 || fd >= MAX_FILES || t->fdt[fd] == NULL) {
    }
    file_close(t->fdt[fd]);
    t->next_fd = fd;
    t->fdt[fd] = NULL;
}