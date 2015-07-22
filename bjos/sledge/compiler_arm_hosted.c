#define CODESZ 4096
#include <unistd.h>
#include <fcntl.h>

int compile_for_platform(Cell* expr, Cell** res) {
  code = mmap(0, CODESZ, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
  memset(code, 0, CODESZ);
  jit_init(512);
  //cpool_idx = CODESZ/2; // 128 ops gap
  //code_idx = 0;

  int tag = compile_expr(expr, NULL, TAG_ANY);
  jit_ret();

  FILE* f = fopen("/tmp/test","w");
  fwrite(code, CODESZ, 1, f);
  fclose(f);

  // disassemble
  system("arm-linux-gnueabihf-objdump -D -b binary -marmv5 /tmp/test >/tmp/disasm");
  int fd = open("/tmp/disasm",O_RDONLY);
  char buf[1024];
  int buflen;
  while((buflen = read(fd, buf, 1024)) > 0)
  {
    write(1, buf, buflen);
  }
  close(fd);
  
  int mp_res = mprotect(code, CODESZ, PROT_EXEC|PROT_READ);
  
  funcptr fn = (funcptr)code;
  *res = (Cell*)fn();
  //printf("pointer result: %p\n",*res);
  //printf("pointer value: %p\n",((Cell*)*res)->value);

  return 1;
}