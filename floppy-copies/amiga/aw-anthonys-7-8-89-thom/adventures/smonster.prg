// starting section of code - remember, this stuff only happens
   once!  //
backpic 2 graphics/screen1
prompt What now, Cap'n Ned?
font topaz.font 9

ty:You are the dashing space ace, ^nCaptain Ned.
ty:You are on the space freighter Zam-Zam.
ty:Something EVIL is trying to get in!
ty:
monsterin=10
oldroom=room
loadsound sounds/beeps
disg graphics/console
playsound sounds/beeps 0 64

MAIN
// main code - this stuff happens every time a player types in
   anything besides a direction (movement) or "inventory" //

// deal with picking up objects (in this case, only the fire extingisher
   can be picked up, and then only after it has been found) //

v:get,take,grab
  n:extinguisher,can,sprayer
    if room contains extinguisher
      ty:okay!
      rm- extinguisher
      in+ extinguisher
      update
      done
    else
      ty:You don't see that here.
      done
    endif

  n:computer,screen
    if room con computer
      ty:You can't budge it.
      done
    else
      ty:You don't see that here.
      done
    endif
  nend

// deal with setting down objects:
   notice that since we didn't allow the player to pick up the
   computer, we don't have to deal with it in any special way
   here. //
v:drop,leave
  n:extinguisher,can,sprayer
    if inventory contains extinguisher
      ty:okay!
      rm+ extinguisher
      in- extinguisher
      update
      done
    else
      ty:You aren't holding that.
      done
    endif
  nend
  ty:You aren't holding that.
  done
vend

// deal with use of fire extinguisher //
v:fire,use
  n:extinguisher,sprayer,can
    if inv con extinguisher
      ty:The fire extinguisher sprays out a dense, white fog.
      if alienfound=1
        ty:The space alien screams and expires!
        ty:You have won this adventure!^h
        end
      endif 
      done
    else
      ty:You don't have that!
      done
    endif
  nend
vend

v:wait,rest
  ty:Time passes...
  done
vend

// This is the end of MAIN code: if the program reaches this point,
   the message "I don't understand that." will be printed and
   the BACKGROUND code will NOT be executed.   //

BACKGROUND
// this code is executed every time the player types in anything
   (except for the inventory command and mis-typed stuff which
   would cause program flow to reach the end of the MAIN code //

if oldroom=room
else
  if room con extinguisher
    disg graphics/exting.
  endif

  if room con computer
    disg graphics/console
  endif

  if room=1
    playsound sounds/beeps 0 64
  endif
  if room=2
    playsound sounds/beeps 0 44
  endif
  if room=3
    playsound sounds/beeps 0 24
  endif

endif

if monsterin>0
  monsterin=monsterin - 1
  endif

if alienfound=1
  ty:The alien devours you messily!
  ty:You have lost this adventure.^h
  end
endif

if room=3 and monsterin=0
  disg graphics/alien
  ty:The alien creature is here!
  alienfound=1
endif

oldroom=room
