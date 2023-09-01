#include <u.h>
#include <libc.h>

#include <draw.h>
#include <keyboard.h>
#include <mouse.h>
#include <bio.h>
#include <thread.h>

#include <player.h>

Point charoffs;
int xredraw;

enum
{
	STACK = 2048,
};

void
usage(){
	sysfatal("usage: game [-x 100 -y 200]");
}

void
timerfn(void* c){
	uint cd;
	cd = recvul(c);
	for(;;){
		sleep(cd);
		sendul(c, cd);
	}
}

/*
void
redraw(Image* screen)
{
	Rectangle r = Rect(0,0,Dx(screen->r), Dy(screen->r));
	charoffs.x=(Dx(r)-5)/2;
	charoffs.y=(Dy(r)-5)/2;
	xredraw=1;
}
*/


/*proc*/
/*
void
kbdinproc(void* C){
	Channel* dat, *ctl;
	int fd;
	long n, j;

	char buf[256];
	Rune m;


	dat = C;
	ctl = recvp(dat);
	Player* reimu = recvp(C);

	fd = open("/dev/kbd", OREAD);
	n=j=0;
	for(;;){
		memset(buf, 0, 256);
		n = read(fd, buf, 256);

		ulong CFkrep = 0;

		if(n==j)
			sendul(ctl, CFkrep);
		else {
			j = n;
		}

		if(buf[0] == 'c'){
			if(chartorune(&m, &buf[1]) != Runeerror){
				switch(m){
					case Kup:
						reimu->y = reimu->y + 50;
						reimu->pos = Rect(reimu->x, reimu->y, reimu->w, reimu->h);
				
						break;
					case Kdown:
						reimu->y = reimu->y - 50;
						reimu->pos = Rect(reimu->x, reimu->y, reimu->w, reimu->h);
				
						break;
					case Kright:
						reimu->x = reimu->x + 25;
						reimu->pos = Rect(reimu->x, reimu->y, reimu->w, reimu->h);
			
						break;
					case Kleft:
						reimu->x = reimu->x - 25;
						reimu->pos = Rect(reimu->x, reimu->y, reimu->w, reimu->h);
				
						break;
					default:

						break;
				}
			}
		}
	}
}
*/

/*
void
reimugraphics(void* C)
{
	Image* pp = recvp(C);
	Player* reimu = recvp(C);
	freeimage(pp);
	pp = allocimage(display, reimu->pos, RGB16, 0, DRed);
	draw(screen, screen->r, pp, nil, ZP);
	redraw(screen);

}
*/

void
threadmain()
{	
	Image* bg;
	Player reimu;
	
	Mouse Mouse;
	Mousectl* mc;
	Channel* t;

	Channel* kbdchan;
	Channel* imagechan;

	Alt a[] = {
		{nil, &Mouse, CHANRCV},
		{nil, nil, CHANRCV},
		{nil, nil, CHANRCV},
		{nil, nil, CHANEND},
	};

	if(initdraw(nil,nil,"touhou")<0) 
		sysfatal("initdraw");

	if(getwindow(display, Refmesg) < 0) 
		sysfatal("getwindow");

	mc = initmouse(nil, screen);

	t = chancreate(sizeof(ulong), 0); /*sizeof(Mouse) */
	proccreate(timerfn, t, 1024);
	sendul(t, 11);

	a[0].c = mc->c;
	a[1].c = mc->resizec;
	a[2].c = t;

	bg = allocimage(display, screen->r, RGBA32, 1, DYellowgreen);
	reimu.r = Rect(0,0,32,64);
	reimu.img = allocimage(display, reimu.r, RGBA32, 0, DRed);
	reimu.p = Pt( screen->r.min.x + (Dx(screen->r) / 2) -
	16,screen->r.max.y - 128);

	for(;;)
	switch(alt(a)){
		/*
		case 0:
			if((Mouse.buttons & 1) == 1)
				fprint(2, "M : %P\n", Mouse.xy);
			break;
		*/
		case 1:
			if(getwindow(display, Refmesg)<0)
				sysfatal("getwindow");

			reimu.p = Pt( screen->r.min.x + (Dx(screen->r)
			/ 2) - 16,screen->r.max.y - 128);


			break;
		case 2: /*Redraw Timer*/
			draw(screen, screen->r, bg, nil, ZP);
			draw(screen, rectaddpt(reimu.r, reimu.p) , reimu.img, nil,
			ZP);
			flushimage(display, 1);
			break;
	}

}
