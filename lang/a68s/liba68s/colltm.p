20600 #include "rundecs.h"
20610     (*  COPYRIGHT 1983 C.H.LINDSEY, UNIVERSITY OF MANCHESTER  *)
20620 (**)
20630 (**)
20640 PROCEDURE ERRORR(N :INTEGER); EXTERN;
20650 PROCEDURE GARBAGE(ANOBJECT:OBJECTP); EXTERN;
20660 FUNCTION NEXTEL(I: INTEGER; VAR PDESC1: PDESC): BOOLEAN; EXTERN;
20670 PROCEDURE PCINCRSLICE(MULT: OBJECTP; VAR APDESC: PDESC; INCREMENT: INTEGER); EXTERN;
20680 PROCEDURE PCINCRMULT(ELSPTR: OBJECTP; INCREMENT: INTEGER); EXTERN;
20690 PROCEDURE FORMPDESC(OLDESC: OBJECTP; VAR PDESC1: PDESC); EXTERN;
20700 (**)
20710 (**)
20720 FUNCTION COLLTM(TEMP: NAKEGER; SOURCEMULT: OBJECTP; OFFSET: OFFSETRANGE): ASNAKED;
20730 (*PCOLLTOTAL+4*)
20740   VAR DESTMULT: OBJECTP;
20750       SOURCELS: OBJECTP;
20760       PDESC1: PDESC;
20770       COUNT: INTEGER;
20780     BEGIN
20790     WITH TEMP DO WITH NAK DO
20800       BEGIN
20810       DESTMULT := STOWEDVAL;
20820       WITH SOURCEMULT^ DO
20830         FOR COUNT := 0 TO ROWS DO WITH DESCVEC[COUNT] DO
20840           IF (LI<>DESTMULT^.DESCVEC[COUNT].LI)
20850              OR (UI<>DESTMULT^.DESCVEC[COUNT].UI) THEN
20860                 ERRORR(RMULASS);
20870       SOURCELS := SOURCEMULT^.PVALUE;
20880       COUNT := OFFSET;
20890       IF SOURCEMULT^.BPTR<>NIL THEN (*A SLICE*)
20900         BEGIN
20910         FORMPDESC(SOURCEMULT, PDESC1);
20920         PCINCRSLICE(SOURCEMULT, PDESC1, +INCRF);
20930         WITH POINTER^ DO
20940           WHILE NEXTEL(0, PDESC1) DO WITH PDESC1 DO WITH PDESCVEC[0] DO
20950             BEGIN
20960             MOVELEFT(INCPTR(SOURCELS, PP), INCPTR(POINTER, COUNT), PSIZE);
20970             COUNT := COUNT+PSIZE;
20980             END;
20990         END
21000       ELSE (*NOT A SLICE*)
21010         BEGIN
21020         PCINCRMULT(SOURCELS, +INCRF);
21030         MOVELEFT(INCPTR(SOURCELS, ELSCONST), INCPTR(POINTER, COUNT), SOURCELS^.D0);
21040         END;
21050       POINTER := INCPTR(POINTER, COUNT-OFFSET);
21060       COLLTM := ASNAK;
21070       END;
21080     IF FPTST(SOURCEMULT^) THEN GARBAGE(SOURCEMULT)
21090     END;
21100 (**)
21110 (**)
21120 (*-02() BEGIN END ; ()-02*)
21130 (*+01()
21140 BEGIN (*OF MAIN PROGRAM*)
21150 END (*OF EVERYTHING*).
21160 ()+01*)