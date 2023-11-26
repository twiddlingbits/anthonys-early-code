
in+ shovel
in+ teleporter

MAIN

v:dig
  if room=1
    ty:You find nothing.
    done
  endif
  if room=2 or room=3
    ty:You find a sack of gold!
    done
  endif
vend

v:use
  n:teleporter
    ty:ZAPPP!
    if room=1 or room=3 and zapper=zaptest
      toroom 2
      sdone
    else
      toroom 1
      sdone
    endif
  nend



