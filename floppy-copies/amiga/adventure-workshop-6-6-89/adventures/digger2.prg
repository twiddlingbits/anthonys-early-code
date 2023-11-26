
in+ shovel

MAIN

if flagnd=1
  ty:noinput successful!
  flagnd=0
  done
endif

v:dig
  if room=1
    ty:You find nothing.
    done
  endif
  if room=2
    ty:You find a sack of gold!
    done
  endif
vend

v:open
  n:door
    ty:The door is now open.
    connect 1 S 2 N
    update
    done
  nend
vend

v:close,shut
  n:door
    ty:The door is now closed.
    connect 1 S 0 N
    connect 2 N 0 N
    ty:update
    update
    ty:done
    done
  nend
vend

v:ndone
  flagnd=1
  ty:noinput
  noinput
  inject garbage line!
  ty:!!! done !!!!
  done
  

v:quit,end,stop
  ty:Ending.^h
  end


