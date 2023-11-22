1 REM
2 REM Test cases for A. Wood's BASIC Interpreter
3 REM
4 ' This is also REM
5  ' goto 7300
9 ::::::::
10 cls:print 2.2+5.5:run 11
11 print "Zone 1","Zone 2","Zone 3","Zone 4","Zone 5"
12 print "Zone 1",,"Zone3
13 print@266,pos(0)
14 for x=120 to 192:printchr$(x);:next:print
15 print"a"TAB(16)"B"TAB(32)"C"TAB(48)"D"
16 REM input a$,a:print using a$;a:goto 16
20 if 2.2+5.5 = 7.7 then print "1. okay"
22 if 3 e - 1 = .3 then print "1.1 okay" else print ".a error":stop
25 if 1+2*3^(1-1)-3 = 0 then print "2. okay"
26 if 4^.5=2print"2.1 okay" else print "0.1 error"
27 if 4[.5=2print"2.2 okay" else print "0.2 error"
30 if 1 < 2 then print "3. okay"
40 if 1<2 OR 1>2 then print "4. okay" else print ".3 error":stop
50 if 1<2 AND 1>2 then print "1. error":stop
60 if 1<2 AND NOT (1>2) print "5. okay" else print".4 error":stop
80 if 1<=1 print "6. okay"
90 if 2<>2 print "2. error"
95 if 0-1 < 0 OR 0-1 > 7 then print "5.1 okay" else print ".5 error":stop
110 if 1*2 < 5 then print "7. okay"
120 if 0=0 then 140
130 print "3. error"
140 print "8. okay"
150 print "Should be a five here:";5
160 print "Should be an empty line"
170 print
180 print -99;"should be -99"
190 print "Should be all ";
200 print "on one line"
210 print "Should be abc: a";:print"b";:print"c"
220 if 1=2 then print "4. error":stop else print "9. okay"
225 if 1=1 then print "9.5 okay" else print "4.5 error":stop
230 if 1=2 then if 1=1 print "5. error" else print "6. error" else print "10. okay"
240 if 1=2 then if 1=1 print "7. error" else print "8. error":stop else if 1=1 print "11. okay" else print "9. error":stop
250 if 1=2 then print "9. error" else 270
260 print "10. error"
270 print "12. okay"
280 if a*5=0 then print "13. okay" else print "11. error":stop
290 let a1=2
300 b = 5
310 if a1*b = 10 print "14. okay" else print "12. error":stop
320 b = 1
330 if a1*b = 2 then print "15. okay" else print "13. error":stop
350 REM
360 REM string tests
370 REM
375 a$=("X)X"+"fred"):if a$="X)Xfred" print"15.1 okay"elseprint"13.1 error":stop
380 print "this"+"should"+"be on one line"
390 a$="String A"
400 print a$
410 b$="String B"
420 print b$
430 print a$+b$
440 c$=a$+b$
450 print "should be 'StringAStringB': ";c$
460 c$="new string c"
470 print c$
480 REM
490 REM more complicated IF-THEN & LET test
500 REM also tests variable names that are > 2 char (extra chars are not significant)
505 REM
510 count = 10
520 print count;
530 count = count-1
540 if count > 0 then 520
550 print 
560 ' if "fred" print "true" else print"false"
565 if 1E4+1E3=11000 then print "15.2 okay" else print "13.2 error":stop
566 if 1 e - 1 + 1D+1 = 10.1 then print "15.3 okay" else print"13.3 error":stop
570 if 2*(0+(4+1))=10 print "16. okay" else print "14. error":stop
580 if 1+3*4=13 print "17. okay" else print "15. error":stop
582 if &h0+&Hf+&HAF25=44852 print "17.1 okay" else print "15.1 error":stop
590 if "abc" = "abc" then print "18. okay" else "16. error":stop
591 if "abc" < "b" then print "19. okay" else "17. error":stop
592 if "z" > "abc" then print "20. okay" else "18. error":stop
600 if 1 < ("a" = ("b")) print "19. error" :stopelse print "21. okay"
610 goto 630:print "20. error"
620 print "21. error"
630 print "22. okay"
650 gosub 670
660 goto 730
670 print "23. okay"
680 gosub 700:return
700 print "24. okay"
710 return
720 print "22. error":stop
730 print "25. okay"
740 REM
750 REM Test FOR NEXT STEP
760 REM
762 A$="":FORX=10TO0STEP-1:A$=A$+"*":NEXT
763 IF A$="***********" PRINT "25.1 okay" ELSE PRINT "22.1 error":stop
765 print"for-next test 1 to 10 step 2"
770 for x=1 to 10 step 2
780 print x;" ";
790 next x
800 print 
805 print"for-next test 1 to 5 outside 1 to 3 inside"
810 for x=1 to 5
820 for y=1 to 3
830 print x;",";y
840 next 
850 next
855 print"for-next test 2 to 3"
860 for x=1 to 5
870 for x=2 to 3
880 print x
890 next
895 print"for-next test A"
900 FOR X=1 to 5
910 FOR Y=1 to 10
911 print x;", ";y
920 NEXT X
925 print"for-next test B"
930 FOR X=1 to 5
940 FOR Y=1 to 10
945 print x;", ";y
950 IF Y < 3 THEN NEXT Y
960 NEXT X
970 REM
980 REM Test Arrays
990 REM
1000 a=1
1010 a(1)=2
1020 if a=1 AND a(1)=2 print "26. okay" else print "23. error":STOP
1030 dim b2(1)
1040 print b2(0);",";b2(1)
1045 print "setting..."
1050 dim c(5,4,6)
1060 for x=1 to 5
1070 for y=1 to 4
1080 for z=1 to 6
1085 ' print x;", ";y;", ";z
1090 c(x,y,z) = k
1100 k=k+1
1110 nextz,y:next
1120 k=0:print "checking...";
1160 for x=1 to 5
1170 for y=1 to 4
1180 for z=1 to 6
1190 if c(x,y,z) <> k then print "24. error":STOP
1200 k=k+1
1210 next z,y,x:print "27. okay":forx=1to500:next
1213 forx=1to10:fory=1to5
1214 nexty,x:gosub1217
1216 goto 1220
1217 print"27.1 okay":return
1220 dim a$(5),d#(20)
1230 print "--:";a$(1);":--"
1240 for x=1 to 5:a$(x)="25. error":next
1250 a$(2)="28. okay"
1260 print a$(2)
1270 d#(20) = 500:if d#(20)=500thenprint"28.2. okay"elseprint"25.2 error"
1280 A(A(1))=2:C(A(1),A(2),A(3))=2
1300 REM
1310 REM Test built in functions
1320 REM
1330 if asc("A")=65 then print "29. okay" else print "26. error":stop
1340 if chr$(65)="A" then print "30. okay" else print "27. error":stop
1350 if fre(a$)=48000 then print "31. okay" else print "28. error":stop
1355 s$="1234567890"
1360 if len(s$)=10 then print "32. okay" else print "29. error":stop
1370 if left$(s$,3)="123" then print "33. okay" else print "30. error":stop
1380 if right$(s$, 3)="890" then print "34. okay" else print "31. error":stop
1390 if mid$(s$, 3, 4) = "3456" then  print "35. okay" else print "32. error":stop
1400 if str$(123.456) = " 123.456" then  print "36. okay" else print "33. error":stop
1410 if string$(5,"*")="*****" then  print "37. okay" else print "34. error":stop
1420 if val("1"+".5")=1.5 then  print "38. okay" else print "35. error":stop
1430 if mid$(s$,2)="234567890" then print "38.2 okay" else print "35.2 error":stop
1440 if instr("LINCOLN","INC")=2 then print "38.3 okay" else print"35.3 error":stop
1450 if instr(3,"1232123","12")=5print"38.4 okay" else print "35.4 error":stop
1451 ifmem=48338print"38.5 okay"elseprint"35.5 error":stop
1460 REM
1461 REM Test different data types
1462 REM
1463 a%=5:b%=2
1470 a!=10.5
1480 a#=3.2
1490 if a%+a%=10 print "39. okay" else print "36. error":stop
1500 if a!+a!=21 print "40. okay" else print "37. error":stop
1510 if a#+a#=6.4 print "41. okay" else print "38. error":stop
1520 if a%/b%=2.5 print "42. okay" else print "39. error":stop
1530 FOR I%=1 to 10 step 2:next i%:if i%=11 print "43. okay" else print "40. error":stop
1540 if 2# or 4% = 6 print "43.a okay" else print "40.a error":stop
2000 REM
2010 REM test math functions
2020 REM
2030 if sin(0)=0 print "43.1 okay" else print "40.4 error":stop
2040 random:for x=1 to 10:if rnd(0) <0 OR rnd(0) > 1.0 then print "40.1 error":stop
2050 random:if rnd(50) < 1 OR rnd(50) >  50 then print "40.2 error":stop
2055 forx=1to1000:if rnd(40)<>40next:print"40.25 error":stop
2060 DX%=128+35
2070 IF(DX%/64-INT(DX%/64))*64+LEN("1234567890")>39THEN PRINT "43.2 okay"ELSE PRINT"40.2 error":stop
3000 REM
3010 REM Test INPUT
3020 REM
3030 input "enter one number";a#:print a#
3040 print "enter a string";:LINEinput a$:print a$
3045 input"enter two strings";a$,b$:print a$:printb$
3050 input "enter three numbers";a,b,c:print a:print b:print c
4000 REM
4010 REM Test DEFXXX
4020 REM
4030 defint i:defsgn j:defdbl k
4040 defstr o-q,r
4050 i=2:r="44. okay":q=r:p=q:o=p:printp
4060 i=2.2+1.1:if i=3 print"45. okay"elseprint"41. error":stop
5000 REM
5010 REM READ and DATA test
5020 REM
5030 Read a:Read b,c:Read r
5035 read q
5040 data 1,2,-0.4 e - 2:print "46. okay"
5050 data "a string",noquotestring
5060 if a=1 AND b=2 AND c=-.004 AND r="a string" AND q="noquotestring" print "47. okay" else print "42. error":stop
5070 restore:Read x: if x=1 then print"48. okay" else print "43. error":stop
5100 REM
5110 REM TEST ON...
5120 REM
5130 ON 0 GOTO 5200,5200,5200
5140 print"49. okay"
5150 ON 3 GOTO 5200,5200
5160 print"50. okay"
5170 a=1:On A GOTO 5180
5175 goto 5200
5180 print "51. okay"
5190 ON 2 GOSUB 5200,5210
5195 goto 5230
5200 print"44. error":stop
5210 print"52. okay"
5220 return
5230 ON ERROR GOTO 5250
5240 goto 5300
5250 if err=20 and erl=5310 print "54. okay" else print "46. error":stop
5260 RESUME 5400 
5270 end
5300 print"53. okay"
5310 a=1/0
5320 print"45. error":stop
5400 print"55. okay"
5430 ON ERROR GOTO 5450
5440 goto 5500
5450 if err=20 and erl=5510 print "57. okay" else print "48. error":stop
5460 a=1:RESUME  
5470 end
5500 print"56. okay":a=0
5510 a=1/a
5520 print"58. okay"
5530 ON ERROR GOTO 5560
5540 a=0/0:goto 5570
5560 print"59. okay":resume next
5570 print"60. okay"
5600 REM
5610 clear 50:a=1:clear 50:if a=0 print "61. okay" else print"49. error":stop
5999 END
7000 REM Poke, Set, Reset
7010 REM
7020 cls:print"here":print:print:print:print:for x=0 to 255:poke 15360+x,x:next
7030 input"press return to continue";a
7040 cls:x=0:for y=0 to 47:set(x,y):x=x+1:next
7050 print:print:print:print:input"press return to continue";a
7060 x=0:for y=0 to 47:reset(x,y):x=x+1:next

7100 REM INKEY
7110 a$=inkey$
7120 if a$<>"" then print asc(a$)
7130 goto 7110
7200 REM MORE INKEY
7210 DEFSTRA
7230 A=INKEY$:IFA="Y"THENENDELSEIFA="N"THENENDELSE7230
7300 REM
7310 REM PEEK KEYBOARD
7320 print peek(14337),peek(14338), peek(14340), peek(14344)
7330 print peek(14352), peek(14368), peek(1440), peek(14464)
7340 print : goto 7320
8998 REM
8999 END
9000 print"Guess the number between 1 and 100!"
9010 random:n=rnd(100)
9020 input x
9030 if x < n print"bigger":goto 9020
9040 if x > n print"smaller":goto 9020
9050 print"You got it!!"
9060 END
9500 a$=string$(63,"*")
9505 for x=1 to 100
9510    print a$
9520 next
9530 print "done"
9540 goto 9540
9999 end


