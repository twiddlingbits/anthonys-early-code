BACKPIC 1 graphics/type1.pic
PROMPT What now?
FONT topaz.font 9


ty:You are in the world's smallest adventure.  More specificly,
ty:you are in a large hole from which you cannot escape.  You
ty:have a flare, a match, and good intentions.

MAIN

v:get,take,grab
  n:match,flare
    if RM$ noun
      ty:okay!
      rm- noun
      in+ noun
      done
    endif
  nend

v:drop,leave
  n:match,flare
    if IN$ noun
      ty:okay!
      rm+ noun
      in- noun
      done
    endif
  nend

v:wait,rest
  ty:Time passes...
  done

v:shout,call,yell
  ty:No one can hear you.
  done 

v:light,fire,start
  n:match
    if matchflag=2
      ty:You can't light a burnt out match!
      done
    endif
    if matchflag=1
       ty:The match is already burning!
       done
    endif
    ty:the match is now lit!
    matchflag=1
    done
  n:flare
    if matchflag=1
      ty:The flare shoots off into the sky!
      matchflag=2
      if chopper>3
        ty:The helicopter sees your flare! You are rescued!
        ty:You have won this adventure.
        ty:^h
        end
      else
        ty:No one notices.
      endif
    else
      ty:Your match is not lit!
    endif
    done
vend

BACKGROUND

chopper=chopper + 1
if chopper>3 and chopper<6
    ty:You hear a helicopter coming closer.  They must be 
    ty:searching for you!
    done
    endif
if chopper=6
  ty:You see the helicopter overhead!
  done
  endif
if chopper>6
  ty:The chopper flies away!  You starve and die horribly!
  ty:You have lost this adventure.
  ty:^h
  end
  endif


