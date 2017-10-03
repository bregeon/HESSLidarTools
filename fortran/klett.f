       PROGRAM HLIDAR
       PARAMETER (NWPAWC = 1500000)
       COMMON/PAWC/PAW(NWPAWC)
c       COMMON/QUEST/IQUEST(100)
       character*180 fname,runlist
       real a2(11000),a3(11000),a1(11000)
       real av2(2000),av3(2000),tal(49),overf(49)
       real av1(2000)
       call hlimit(NWPAWC)
       call hropen(1,'LidOut','lidpaw','N',1024,istat)
       print *,'Enter Runlist:'
       read (*,'(a30)')runlist
       open(unit=12,file=runlist,status='old')
       open (unit=4,file='klett',status='old')
       open (unit=11,file='over.txt',status='old')
234    format(a5,i5,a30)
       
       kli=0
       pi=3.14159
       angle=15.
       
       do kf=1,49
         read (11,*) tal(kf),overf(kf)
	     overf(kf)=overf(kf)/100.
       enddo

       do kl=1,10000
          read(12,'(a60)',end=767,err=767) fname
*          print *,'Running file:',fname
          open (unit=2,file=fname,status='old')
          ibgr=0
          tmpa=0.
          tmpb=0.
          ntir=1
          kli=0
          itir=12000
          read(2,'(a3,1x,a3,1x,i2,1x,a8,1x,i4)')day,month,
     +             idate,time,iyear
     	  read(fname,234)ie1,ie3,ie2
c          print *,'Lidar file:',ie3
          do i=1,itir
              read(2,*,end=888,err=888) alti,a355,a532
              if(alti .gt. 20.) then
                  tmpa=abs(a355)+tmpa
                  tmpb=abs(a532)+tmpb
                  ibgr=ibgr+1
                  goto 888
		  endif
            if(alti.lt. 0.8) goto 888
	    if(alti.gt.20.) goto 888
            a3(ntir)=abs(a532)
            a2(ntir)=abs(a355)
            a1(ntir)=(alti+1.8)*cos(pi*angle/180.)
            ntir=ntir+1
888         continue

           enddo
        br355=abs(tmpa/ibgr)
	    br532=abs(tmpb/ibgr)
        kli=kli+1
c	    print *,'Backgrounds :',br355,br532
	    iter=0
	    inx=0
	    ist=20
	  do k=1,ntir,ist
	    tim1=0.
	    tim2=0.
	    tim3=0.
	    do l=1,ist
	     tim1=tim1+a1(k-1+l)
	     tim2=tim2+a2(k-1+l)
	     tim3=tim3+a3(k-1+l)
	    enddo
	    tim1=tim1/ist
	    tim2=tim2/ist
	    tim3=tim3/ist
	    inx=(k-1+l)/ist
	    av1(inx)=tim1
	    av2(inx)=tim2-br355
            av3(inx)=tim3-br532
	    if(inx.lt.50) then
	       av2(inx)=av2(inx)/overf(inx)
	       av3(inx)=av3(inx)/overf(inx)
	    endif
          enddo

          ntir=inx-1
	  iter=0
         call packall(av1,av2,av3,ntir,odtot)
c	 if(odtot.gt.1. .and. odtot.lt.100.)print *,odtot,ie3
        enddo
	
767     call hrout(0,icycle,' ')
        call hrend('LidOut')
        close(0)
        stop
        end
	
	subroutine packall(salt,scr355,scr532,m,odtot355)

	real erp355(m),Natm(m),ap532(m),sum1(m),sum3(m)
	real lr355,lr532,am355(m),am532(m),bm355(m),bm532(m)
	real sc355(m),sc532(m),salt(m),Pzero,Tzero,od355(m),od532(m)
	real bp355(m),ap355(m),erbp355(m),erap355(m),P(m),T(m)
        real erap532(m),erbp532(m),bp532(m)
	real scr355(m),scr532(m),erp532(m)

	iout=50
	coef=0.0003
c	print *,'Calibration height:', salt(m-iout)
	ibgr355=m-iout
	ibgr532=ibgr355
	lr355=50
	lr532=70
	Pzero=1013.
        Tzero=20.
	inorm=5
	sratio1=1
	sratio2=1
        step1=abs(salt(20)-salt(19))

      do ka=1,m
	      salt(ka)=salt(ka)*1000.
	      x=salt(ka)
          P(ka)=1000.5-0.10754*x+0.0000041363*x*x-0.000000000056*x*x*x
	      T(ka)=285.36+0.003203*x-0.00000161*x*x+0.00000000009*x*x*x
C	            +0.0000000000000014*x*x*x*x
	  enddo
c	call usstd(salt,Pzero,Tzero,P,T,m)
        call density(P,T,Natm,m)  
        call rayleigh(Natm,am355,am532,bm355,bm532,m)
 
	do k=1,m
	  a1sq=salt(k)*salt(k)
	  err=((salt(k)-277.777)/222.22)/100.
	  erp355(k)=scr355(k)*err/5.
	  erp532(k)=scr532(k)*err/5.
          scr355(k)=scr355(k)*a1sq
          scr532(k)=scr532(k)*a1sq
	enddo
C**smoothing
       call SGmoving(salt,scr355,sc355,coef,m)
       call SGmoving(salt,scr532,sc532,coef,m)

c***** battenuated
      do i=1,m
      sum1(i)=0
      sum3(i)=0
      end do
      do i=1,m
       do j=1,i
       sum1(i)=sum1(i)+am355(j)*step1*1000
       sum3(i)=sum3(i)+am532(j)*step1*1000
       end do
      end do
c***** normalization
       sig355=0
       sig532=0
       a355=0
       a532=0
       kcount=0
      do i=m-70,m-iout
       sig355=sig355+sc355(i)
       sig532=sig532+sc532(i)
       a355=a355+bm355(i)*EXP(-2*(sum1(i)))
       a532=a532+bm532(i)*EXP(-2*(sum3(i)))
       kcount=kcount+1
      end do
       sig355=sig355/kcount
       sig532=sig532/kcount
       a355=a355/kcount
       a532=a532/kcount

       call klet(ibgr355,lr355,inorm,sratio1,salt,sc355,erp355,
     &       bm355,am355,bp355,ap355,erbp355,erap355,m,bref)
 
       call klet(ibgr532,lr532,inorm,sratio2,salt,sc532,erp532,
     &       bm532,am532,bp532,ap532,erbp532,erap532,m,bref)
 
       do kl=1,ibgr355
       b355=bm355(kl)*EXP(-2*(sum1(kl)))
       b532=bm532(kl)*EXP(-2*(sum3(kl)))
        write(4,*)salt(kl),ap355(kl),erap355(kl),b355*lr355,
     & ap532(kl),erap532(kl),b532*lr532
       enddo
       
       step=0
       odtot355=0.
       odtot532=0.
       do i=1,m
           od355(i)=0.
	   od532(i)=0.
       end do
       od355(1)=ap355(1)
       od532(1)=ap532(1)
       do i=2,ibgr355
       od355(i)=od355(i-1)+ap355(i)
       od532(i)=od532(i-1)+ap532(i)
       odtot355=odtot355+od355(i)
       odtot532=odtot532+od532(i)
      end do
      odtot355=odtot355/(salt(ibgr355)-salt(1))
      odtot532=odtot532/(salt(ibgr532)-salt(1))
c      print *,odtot355,odtot532
      if((odtot355.gt.0.0001 .and. odtot355.lt.30.)
     &  .and. (odtot532.gt.0.00001 .and. odtot532.lt.30.))then
        angc=-(log(odtot355)-log(odtot532))/(log(355.)-log(532.))
c	print *,angc
       endif
       end
	 
c***********************************************************************
c*  Given the lidar signal(s), the constant lidar ratio(lr) and        *
c*  a reference total (bp+bm) backscatter value (bref), this routine   *
c*  calculates the backscatter profile for known molecular atmosphere  *
c*  (am,bm), according to klett's solution of Lidar equation           *
c*  REFERENCES: -Klett J.D., Applied Optics 20, 211-220, 1981          *
c*              -Chazette P., JGR, Vol100, 23195-23207, 1995           *
c***********************************************************************
c     pass:     s=Lidar's range & background corrected signal
c               P=Lidar's background corrected signal
c               lr=Lidar ratio (constant)
c               bref=total (bp+bm) backscatter reference value
c                    calculated from scattering ratio (sr)
c               ix=reference height's tbin
c               step=step of data
c               m=dimension of arrays
c     retrieve: bp=aerosol backscatter coefficient profile
c               ap=aerosol extinction coefficient profile
      subroutine klet(ixk,lrk,inorm,sratio,alt,s,erP,
     &bm,am,bpk,apk,erbpk,erapk,m,bref)
      REAL N
      REAL lrk,bref
      REAL s(m),erP(m)
      REAL am(m),bm(m),apk(m),bpk(m),alt(m)
      REAL Q1temp(m),Q1(m),Q2temp(m),Q2(m),temp(m)
      REAL arith(m),paron(m),temp2(m)
      REAL erbpk(m),erapk(m)

c     Q1 integral
      Q1temp(ixk)=0.0
      do i=ixk-1,1,-1
       step=alt(i+1)-alt(i)
       Q1temp(i)=Q1temp(i+1)+(.5*step*(am(i+1)+am(i)))
      end do
      do i=1,ixk!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       Q1temp(i)=-Q1temp(i)
      end do
      do i=1,ixk
       Q1(i)=EXP(-2.0*((lrk/8.37758)-1.0)*Q1temp(i))
      end do
c     Q2 integral
      do i=1,ixk
       temp(i)=s(i)*Q1(i)
      end do
      Q2temp(ixk)=0.0
      do i=ixk-1,1,-1
       step=alt(i+1)-alt(i)
       Q2temp(i)=Q2temp(i+1)+(.5*step*(temp(i+1)+temp(i)))
      end do
      do i=1,ixk !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
       Q2temp(i)=-Q2temp(i)
      end do
      do i=1,ixk
       Q2(i)=2.0*lrk*Q2temp(i)
      end do

c     arithmitis-paronomastis
c     FIRST, calculate quantities at the aerosol free region (normalization window)
c     Signal at the aerosol free region
c      sref=s(ixk)
      sref=0.
      k=0
      do i=ixk,ixk+inorm-1
       k=k+1
       sref=sref+s(i)
      end do
      sref=sref/k*1.0
c     other parameters at the aerosol free region

c      amfree=am(ixk)
c      bmfree=bm(ixk)
      amfree=0.0
      k=0
      do i=ixk,ixk+inorm-1 !!
       k=k+1
       amfree=amfree+am(i)
      end do
      amfree=amfree/k*1.0
      bmfree=0.0
      k=0
      do i=ixk,ixk+inorm-1 !!
       k=k+1
       bmfree=bmfree+bm(i)
      end do
      bmfree=bmfree/k*1.0
      bpfree=bmfree*(sratio-1.0)
      
      apfree=bpfree*lrk
      bref=bmfree+bpfree

      do i=1,ixk
       arith(i)=lrk*s(i)*Q1(i)
       paron(i)=((lrk*sref)/(apfree+(lrk/8.37758)*amfree))-Q2(i)
       temp2(i)=arith(i)/paron(i)

      end do
c     final formula
      do i=1,ixk
       test2=lrk/8.37758
       apk(i)=temp2(i)-test2*am(i)
       bpk(i)=apk(i)/lrk
      end do
c       bpf(ixf)=bpfree
c       apf(ixf)=apfree

c***** Error calculations according to Volker, due to statistical error on signal
      s1=lrk
      s2=8.37758

      do i=2,ixk
       step=alt(i)-alt(i-1)
       A=(s1-s2)*(bm(i)+bm(i-1))*step
       Z=(s(i-1))*EXP(A)
       N= (s(i)/ (bpk(i)+bm(i)) ) +
     *s1*( s(i) + s(i-1)*EXP(A))*step
       db1ar=-( ((alt(i)**2)/(bpk(i)+bm(i))) + s1*(alt(i)**2)*step )*Z
       db1=db1ar*erP(i)/(N**2)
       db2ar=(alt(i-1)**2)*EXP(A)*N - s1*(alt(i-1)**2)*step*EXP(A)*Z
       db2=db2ar*erP(i-1)/(N**2)

       erbpk(i)=SQRT((db1**2)+(db2**2))
      end do
      erbpk(1)=erbpk(2)


      do i=1,ixk
       erapk(i)=lrk*erbpk(i)
      end do

      end 


c***********************************************************************
c**  Given atmospheric molecular density, this routine calculates     **
c**  molecular extinction & backscatter coefficients at               **
c**  316, 355, 387, 532 nm. Rayleigh scattering cross-sections are    **
c**  taken from the following reference:                              **
c**  REFERENCE: Anthony Bucholtz, Applied Optics, Vol34, No.15, 1995  **
c***********************************************************************
c      pass:     Natm= numerical density !#/m3
c                m=dimension of the array(Natm)
c      retrieve: molecular extinction and backscatter
c                for 316, 355, 387, 532 nm
c      subroutine rayleigh(Natm,am316,am355,am387,am532,am607,
c     *bm316,bm355,bm387,bm532,bm607,m)
      subroutine rayleigh(Natm,am355,am532,bm355,bm532,m)
      REAL Natm(m)
      REAL am316(m),am355(m),am387(m),am532(m),am607(m),
     *bm316(m),bm355(m),bm387(m),bm532(m),bm607(m)

      do i=1,m
        am316(i)=Natm(i)*4.52410*1E-4*1E-26 !? in m2
        am355(i)=Natm(i)*2.76045*1E-4*1E-26 !? in m2
        am387(i)=Natm(i)*1.92069*1E-4*1E-26 !? in m2
        am532(i)=Natm(i)*5.16645*1E-4*1E-27 !? in m2
        am607(i)=Natm(i)*3.01658*1E-4*1E-27 !? in m2

        bm316(i)=am316(i)/8.73156
        bm355(i)=am355(i)/8.7357830271216097987751531058618
        bm387(i)=am387(i)/8.73156
        bm532(i)=am532(i)/8.7124563445867287543655413271246
        bm607(i)=am607(i)/8.37758
      end do

      end 

c***********************************************************************
c******* This subroutine calculates numerical molecular density  *******
c******* 		 of the atmosphere		         *******
c***********************************************************************
c     pass:     P=pressure (mbar=hPa)
c               T=Temperature (0K)
c               m=dimension of arrays (P,T)
c     retrieve: Natm=numerical density profile (#/m3)
      subroutine density(P,T,Natm,m)
      REAL P(m),T(m),Natm(m),P2

       boltzman=1.380622*1E-23 ! (N m / K)
      do i=1,m
       P2=P(i)*100.0 !convert mb (hPa) to Pa
       Natm(i)=(P2/T(i))*(1/boltzman) ! #/m3
      end do

      end 

c*********************************************************************
c**   US-Standard-Atmosphere 1976, Landolf-Boernstein V/4a, S. 135  **
c**   rair: gas constant                                            **
c**   gearth: Accelaration due to gravity at surface of earth       **
c**   p(1): Pressure at surface, here 1013 hPa                      **
c**   tt(1): Temperature at surface, here 0 C                       **
c**   nlstd: Temperature layers                                     **
c**   alt(j): Altitude                                              **
c**   zstd(2): Tropopause, here 12000m                              **
c**   ttgrd(j): Lapserate in K m-1                                  **
c**   ttgrd(1): 6.5K/1000m in Troposphere                           **
c*********************************************************************
c      pass:     altitude(meters)
c                tzero=temperature at sea level
c                m=dimension of array (alt)
c      retrieve: Pressure profile (mb)
c                Temperature profile (K)
      subroutine usstd(alt,pzero,tzero,p,tt,m)
c-----------------------------------------------------------------------
      REAL alt(m),ttgrd(m),p(m),tt(m),zstd(m)
        rair = 287.05
        gearth = 9.80665
        p(1) = pzero
        tt(1) = Tzero + 273.15
        zstd(1) = 0.
        nlstd = 4
        zstd(2) = 12000.
        zstd(3) = 20000.
        zstd(4) = 35000.
        ttgrd(1) =  0.0065
        ttgrd(2) =  0.0
        ttgrd(3) = -0.001
        zstd(4) = 35000.
        ttgrd(4) = -0.0024
        zstd(5) = 50000.

      do nlay=1,nlstd
      nbot = 0
      do j=1,m
      if ((alt(j).ge.zstd(nlay)).and.(alt(j).lt.zstd(nlay+1))) then
       if (nbot .eq. 0) then
        nbot = j - 1
         if (nbot .eq. 0) nbot = 1
        if (ttgrd(nlay) .eq. 0.) then
         rscale =  rair * tt(nbot) / gearth
         else
         const = gearth / (rair * ttgrd(nlay))
        endif
       endif
       if (ttgrd(nlay) .eq. 0.) then
        tt(j) = tt(nbot)
        p(j) = p(nbot)*exp(-(alt(j) - alt(nbot)) / rscale)
        else 
        factor = 1-ttgrd(nlay)/tt(nbot)*(alt(j)-alt(nbot))
        tt(j) = tt(nbot) * factor
        p(j) = p(nbot) * factor ** const
       endif
      endif
      end do
      end do

      end 
C     Last change:  EG   21 Apr 2008    8:28 pm
c******************************************************************
c******** SAVINSKY-GOLAY digital filter - variable exp window *****
c******************************************************************
c     pass:     alt=altitude
c               sig=raw or range-corrected signal
c               m=dimension of the arrays(alt,signal)
c               coef,icoef: window=icoef+exp(coef*altitude)
c     retrieve: sigfit=smoothed signal
c      subroutine SGmoving(alt,sig,sigfit,sdsigfit,coef,m)
      subroutine SGmoving(alt,sig,sigfit,coef,m)

      REAL alt(m)
      REAL sig(m),sigfit(m),sdsigfit(m),sig1(m),sig2(m)

      REAL windowtemp(m)
      INTEGER window(m),windowh(m)

      ilimit=41 ! limit for the window
c***** Function for calculation of the height dependent window
c***** Convert window to odd number of points
      if (coef.gt.0.0011) then
       coef=0.0011    ! Cannot define exp for greater values of coef
      end if
      do i=1,m
       windowtemp(i)=1.0+EXP(coef*alt(i))
       window(i)=2*INT(REAL(windowtemp(i)/2.0))+1
       if (window(i).gt.ilimit.or.window(i).le.0) then
        window(i)=ilimit
       end if
       windowh(i)=INT((window(i)-1)/2)
      end do

c***** Apply smoothing
      istart=1+windowh(1)
      iend=m-int(ilimit/2)
      do i=1,istart-1
       sigfit(i)=sig(i)
      end do
      do i=istart,iend
      ndata=0
       do j=i-windowh(i),i+windowh(i)
        ndata=ndata+1
        sig1(ndata)=sig(j)
       end do
        call callsg(window(i),windowh(i),sig1,sigfit(i))
      end do
      do i=iend+1,m
       sigfit(i)=sig(i)
      end do

c***** Find errors, assuming a polynomial fit of 2nd degree (optional)
      do i=1,istart-1
       sdsigfit(i)=0.
      end do
      do i=istart,iend
      ndata=0
       do j=i-windowh(i),i+windowh(i)
        ndata=ndata+1
        sig1(ndata)=sig(j)
        sig2(ndata)=sigfit(j)
       end do
        sd1=0.
       do k=1,window(i)
        sd1=sd1+(sig1(k)-sig2(k))**2
       end do
        sdsigfit(i)=SQRT(ABS(sd1/window(i)-2)) ! where order of polynomial=2
      end do
      do i=iend+1,m
       sdsigfit(i)=0.
      end do
      end subroutine

      subroutine callsg(iwin,iwinh,signal,sfit)
      REAL c(iwin)
      REAL signal(iwin),sfit
      sfit=0.
      call savgol(c,iwin,iwinh,iwinh,0,2)
       kc1=iwinh+2
       do i=1,iwinh+1
       kc1=kc1-1
        sfit=sfit+c(kc1)*signal(i)
       end do
       kc2=iwin+1
       do i=iwinh+2,iwin
       kc2=kc2-1
        sfit=sfit+c(kc2)*signal(i)
       end do
      end subroutine


      SUBROUTINE savgol(c,np,nl,nr,ld,m)
      INTEGER ld,m,nl,np,nr,MMAX
      REAL c(np)
      PARAMETER (MMAX=6)
CU    USES lubksb,ludcmp
      INTEGER imj,ipj,j,k,kk,mm,indx(MMAX+1)
      REAL d,fac,sum,a(MMAX+1,MMAX+1),b(MMAX+1)
      if(np.lt.nl+nr+
     *1.or.nl.lt.0.or.nr.lt.0.or.ld.gt.m.or.m.gt.MMAX.or.nl+nr.lt.m) 
     *print *, 'bad args in savgol'
      do 14 ipj=0,2*m
        sum=0.
        if(ipj.eq.0)sum=1.
        do 11 k=1,nr
          sum=sum+float(k)**ipj
11      continue
        do 12 k=1,nl
          sum=sum+float(-k)**ipj

12      continue
        mm=min(ipj,2*m-ipj)
        do 13 imj=-mm,mm,2
          a(1+(ipj+imj)/2,1+(ipj-imj)/2)=sum
13      continue
14    continue
      call ludcmp(a,m+1,MMAX+1,indx,d)
      do 15 j=1,m+1
        b(j)=0.
15    continue
      b(ld+1)=1.
      call lubksb(a,m+1,MMAX+1,indx,b)
      do 16 kk=1,np
        c(kk)=0.
16    continue
      do 18 k=-nl,nr
        sum=b(1)
        fac=1.
        do 17 mm=1,m
          fac=fac*k
          sum=sum+b(mm+1)*fac
17      continue
        kk=mod(np-k,np)+1
        c(kk)=sum
18    continue
      return
      END

      SUBROUTINE lubksb(a,n,np,indx,b)
      INTEGER n,np,indx(n)
      REAL a(np,np),b(n)
      INTEGER i,ii,j,ll
      REAL sum
      ii=0
      do 12 i=1,n
        ll=indx(i)
        sum=b(ll)
        b(ll)=b(i)
        if (ii.ne.0)then
          do 11 j=ii,i-1
            sum=sum-a(i,j)*b(j)
11        continue
        else if (sum.ne.0.) then
          ii=i
        endif
        b(i)=sum
12    continue
      do 14 i=n,1,-1
        sum=b(i)
        do 13 j=i+1,n
          sum=sum-a(i,j)*b(j)
13      continue
        b(i)=sum/a(i,i)
14    continue
      return
      END

      SUBROUTINE ludcmp(a,n,np,indx,d)
      INTEGER n,np,indx(n),NMAX
      REAL d,a(np,np),TINY
      PARAMETER (NMAX=500,TINY=1.0e-20)
      INTEGER i,imax,j,k
      REAL aamax,dum,sum,vv(NMAX)
      d=1.
      do 12 i=1,n
        aamax=0.
        do 11 j=1,n
          if (abs(a(i,j)).gt.aamax) aamax=abs(a(i,j))
11      continue
        if (aamax.eq.0.) print*, 'singular matrix in ludcmp'
        vv(i)=1./aamax
12    continue
      do 19 j=1,n
        do 14 i=1,j-1
          sum=a(i,j)
          do 13 k=1,i-1
            sum=sum-a(i,k)*a(k,j)
13        continue
          a(i,j)=sum
14      continue
        aamax=0.
        do 16 i=j,n

          sum=a(i,j)
          do 15 k=1,j-1
            sum=sum-a(i,k)*a(k,j)
15        continue
          a(i,j)=sum
          dum=vv(i)*abs(sum)
          if (dum.ge.aamax) then
            imax=i
            aamax=dum
          endif
16      continue
        if (j.ne.imax)then
          do 17 k=1,n
            dum=a(imax,k)
            a(imax,k)=a(j,k)
            a(j,k)=dum
17        continue
          d=-d
          vv(imax)=vv(j)
        endif
        indx(j)=imax
        if(a(j,j).eq.0.)a(j,j)=TINY
        if(j.ne.n)then
          dum=1./a(j,j)

          do 18 i=j+1,n
            a(i,j)=a(i,j)*dum
18        continue
        endif
19    continue
      return
      END
