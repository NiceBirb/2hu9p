#include <u.h>
#include <libc.h>
#include <draw.h>
#include <keyboard.h>
#include <mouse.h>
#include <bio.h>
#include <thread.h>

#include <player.h>

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


/*proc*/
void
kbdinproc(void* c){
	Channel* dat, *ctl;
	int fd;
	ulong n, j, ☭;

	char buf[256];
	char rbuf[256];
	
	Rune m;


	dat = c;
	Player* reimu = recvp(c);

	fd = open("/dev/kbd", OREAD);

	for(;;){
		memset(buf, 0, 256);
		n = read(fd, buf, 256);

		if(buf[0] == 'k' || buf[0] == 'K'){
			memset(rbuf, 0, 256);
			memcpy(rbuf, buf, n);
			☭ = n;
		}

		for (j=1; j<☭ && rbuf[j] != '\0'; ++j){
			if(chartorune(&m, &rbuf[j]) != Runeerror){
				switch(m){
					case Kup:
						--reimu->y;
						break;
					case Kdown:			
						++reimu->y;
						break;
					case Kright:
						++reimu->x;
						break;
					case Kleft:
						--reimu->x;
						break;
					case 127:
						threadexitsall(nil);
						break;
					default:
						break;	
					}
				}
			}
		
	}
}

void
threadmain()
{	
	Image* bg;
	Player reimu;
	
	Mouse Mouse;
	Mousectl* mc;
	Channel* t;

	Channel* kbdchan;

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
	proccreate(timerfn, t, STACK);
	sendul(t, 20);

	a[0].c = mc->c;
	a[1].c = mc->resizec;
	a[2].c = t;

	bg = allocimage(display, screen->r, RGBA32, 1, DYellowgreen);
	reimu.Rectangle = Rect(0,0,32,64);
	reimu.img = allocimage(display, reimu, RGBA32, 0, DRed);
	reimu.Point = Pt( screen->r.min.x + (Dx(screen->r) / 2) - 16,screen->r.max.y - 128);

	kbdchan = chancreate(sizeof(Player*), 0);
	proccreate(kbdinproc, kbdchan, STACK);
	sendp(kbdchan, &reimu);

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
			reimu.Point = Pt( screen->r.min.x + (Dx(screen->r) / 2) - 16, screen->r.max.y - 128);
			break;
		case 2: /*Redraw Timer*/
			draw(screen, screen->r, bg, nil, ZP);
			draw(screen, rectaddpt(reimu, reimu) , reimu.img, nil,ZP);
			flushimage(display, 1);
			break;
	}
}
