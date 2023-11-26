
in+ shovel

if room contains shovel
if inventory contains bitch
endif
endif

if room con shovel
if inv con bitch
endif
endif


MAIN

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
    sdone
  nend
vend

v:close,shut
  n:door
    ty:The door is now closed.
    connect 1 S 0 N
    connect 2 N 0 N
    sdone
  nend
vend

v:quit,end,stop
  ty:Ending.^h
  end

