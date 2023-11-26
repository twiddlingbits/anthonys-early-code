
FONT ruby.font 12
BACKPIC 1 graphics/type1.pic
PROMPT Command?

setcolor 12

MAIN


v:graphic
ty:status byte =^
nty:status
  n:x
    DISG graphics/g1
ty:status byte =^
nty:status
    done
  n:b
    DISG graphics/g5
ty:status byte =^
nty:status
    done
  n:c
    DISG graphics/g3
    done
  n:d
    DISG graphics/g4
    done


v:get,take,grab
  n:raygun,tribble,timewarp,axe
    if RM$ noun
      ty:okay!
      rm- noun
      in+ noun
      sdone
    endif
  nend

v:drop,leave
  n:raygun,tribble,timewarp,axe
    if IN$ noun
      ty:okay!
      rm+ noun
      in- noun
      sdone
    endif
  nend

v:random
  RND randnum 14
  ty:----^
  nty:randnum
  ty:
  done

v:save
  n:game
    storegame testgamesave
    done
vend

v:load
  ty:>>2
  n:game
    ty:>>3
ty:status byte =^
nty:status
    loadgame tstgamesave
ty:status byte =^
nty:status
    sdone

v:wait
  n:sound
    ty:start
    playsound df1:flare.snd 1 64
    wait df1:flare.snd
    ty:end
    done
  n:time
    ty:start
    wait 120
    ty:end
    done

v:open
  n:file
ty:status byte =^
nty:status
  open df0:newfile R
ty:status byte =^
nty:status
  done

v:loadsounda
  loadsound sounds/flare.snd
  done

v:loadsoundb
  loadsound sounds/rain1.snd
  done



BACKGROUND

