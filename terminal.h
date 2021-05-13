#ifndef __TERMINAL_H__
#define __TERMINAL_H__

void input(char);
void parse(void);
short getCom(void);
int getArgs(void);
int convBase(char arg[], int argSize);

void mr(void);
void mw(void);
void mc(void);
void ri(void);
void wi(void);
void rd(void);
void wd(void);
void ra(void);
void we(void);
void re(void);
void last(void);
void help(void);
void ver(void);
void sp(void);
void ac(void);
void bl(void);
void sr(void);
void sc(void);
void stop(void);
void bf(void);
void bi(void);
void be(void);
void fw(void);
void fn(void);
void ff(void);
void rp(void);
void dcr(void);
void kp(void);
void ki(void);
void kd(void);
void yr(void);
void kdf(void);
void kf(void);
void ma(void);
void mm(void);
void u_sat_a(void);
void u_sat_b(void);
void u_0(void);
void f_ad(void);
void lim_sat(void);
void valor_k(void);
void comutacao(void);

struct cmd
{
	char com[5];
	short args;
	void(*fn)(void);
	char help[100];
};



#endif
