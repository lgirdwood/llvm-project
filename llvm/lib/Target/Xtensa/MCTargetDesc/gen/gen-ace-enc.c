#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "xtensa-isa.h"
#include "xtensa-isa-internal.h"
extern xtensa_isa_internal xtensa_modules;
static int ibw; static void clr(xtensa_insnbuf b){memset(b,0,ibw*sizeof(xtensa_insnbuf_word));}
static int pick(xtensa_isa isa,xtensa_opcode opc,int*pf,int*ps){
  xtensa_insnbuf s=xtensa_insnbuf_alloc(isa);int nf=xtensa_isa_num_formats(isa),bf=-1,bs=-1,bl=9999;
  for(int f=0;f<nf;f++){int ns=xtensa_format_num_slots(isa,f),ln=xtensa_format_length(isa,f);
    for(int k=0;k<ns;k++){clr(s);if(xtensa_opcode_encode(isa,f,k,s,opc)==0&&ln<bl){bl=ln;bf=f;bs=k;}}}
  *pf=bf;*ps=bs;return bf<0?-1:bl;}
static int build(xtensa_isa isa,xtensa_opcode opc,int f,int s,uint32_t*ov,int nov,unsigned char*o){
  xtensa_insnbuf in=xtensa_insnbuf_alloc(isa),sl=xtensa_insnbuf_alloc(isa);
  int len=xtensa_format_length(isa,f),ns=xtensa_format_num_slots(isa,f);
  clr(in);xtensa_format_encode(isa,f,in);
  for(int i=0;i<ns;i++){clr(sl);
    if(i==s){xtensa_opcode_encode(isa,f,i,sl,opc);int no=xtensa_opcode_num_operands(isa,opc);
      for(int k=0;k<no&&k<nov;k++){uint32_t v=ov[k];xtensa_operand_set_field(isa,opc,k,f,i,sl,v);}}
    else{xtensa_opcode n=xtensa_format_slot_nop_opcode(isa,f,i);if(n!=XTENSA_UNDEFINED)xtensa_opcode_encode(isa,f,i,sl,n);}
    xtensa_format_set_slot(isa,f,i,in,sl);}
  xtensa_insnbuf_to_chars(isa,in,o,len);return len;}
int main(void){xtensa_isa_status e;char*m=0;xtensa_isa isa=xtensa_isa_init(&xtensa_modules,&e,&m);
  ibw=((xtensa_isa_internal*)isa)->insnbuf_size;
  for(int op=0;op<xtensa_isa_num_opcodes(isa);op++){const char*nm=xtensa_opcode_name(isa,op);
    if(!nm||strncmp(nm,"ae_",3))continue; int f,s,len=pick(isa,op,&f,&s); if(len<0)continue;
    unsigned char tm[32];uint32_t z[8]={0};build(isa,op,f,s,z,0,tm);
    int no=xtensa_opcode_num_operands(isa,op);
    printf("%s\t%d\t",nm,len); for(int i=0;i<len;i++)printf("%02x",tm[i]);
    printf("\t%d",no);
    for(int o2=0;o2<no;o2++){ int reg=xtensa_operand_is_register(isa,op,o2);
      int flag=reg?1:0;
      if(!reg){ int ident=1; unsigned sv[4]={1,3,7,13};
        for(int q=0;q<4;q++){ uint32_t v=sv[q]; xtensa_operand_encode(isa,op,o2,&v); if(v!=sv[q]){ident=0;break;} }
        flag = ident?0:2; }
      printf("\t%d:",flag);
      /* input-bit->output-bit perm; print output positions for bits 0..15 (-1 term when field ends) */
      int printed=0;
      for(int b=0;b<16;b++){uint32_t ov[8]={0};ov[o2]=(1u<<b);unsigned char bb[32];build(isa,op,f,s,ov,o2+1,bb);
        int outb=-1; for(int bit=0;bit<len*8;bit++){int by=bit/8,bt=bit%8;if(((tm[by]^bb[by])>>bt)&1){outb=bit;break;}}
        if(outb<0){ if(b==0){} break;} if(printed)printf(","); printf("%d",outb); printed=1; }
    }
    printf("\n");
  }
  return 0;}
