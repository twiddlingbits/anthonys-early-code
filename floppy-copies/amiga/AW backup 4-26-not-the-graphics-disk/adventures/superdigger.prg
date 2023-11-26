// starting section of code - remember, this stuff only happens
   once!  //
ty:This is the Super-Digger adventure.  Try to escape!
ty:

MAIN
// main code - this stuff happens every time a player types in
   anything besides a direction (movement) or "inventory" //

// deal with digging //
v:dig
  if room=2 and keyfound=0
      ty:You found a key!
      keyfound=1
      rm+ key
      sdone
  endif

// deal with picking up objects (in this case, only the key can be
   picked up, and then only after it has been found) //

v:get,take,grab
  n:key
    if RM$ noun
      ty:okay!
      rm- noun
      in+ noun
      done
    else
      ty:You don't see that here.
      done
    endif

  n:trampoline,button
    if RM$ noun
      ty:You can't budge it.
      done
    else
      ty:You don't see that here.
      done
    endif
  nend

// deal with setting down objects:
   notice that since we didn't allow the player to pick up the
   trampoline or button, we don't have to deal with them in any
   special way here. //
v:drop,leave
  n:key,button,trampoline
    if IN$ noun
      ty:okay!
      rm+ noun
      in- noun
      done
    else
      ty:You aren't holding that.
      done
    endif
  nend

v:open,unlock
  n:door
    if room<2 or room>3
        ty:You see no door here!
        done
    else
      if dooropen=1
        ty:The door is already open.
        done
      endif
    endif

    // checking prepositional object here //
    p:key



// This is the end of MAIN code: if the program reaches this point,
   the message "I don't understand that." will be printed and
   the BACKGROUND code will NOT be executed.   //

BACKGROUND
// this code is executed every time the player types in anything
   (except for the inventory command and mis-typed stuff which
   would cause program flow to reach the end of the MAIN code //

cavein=cavein + 1

if cavein=10 or cavein=15
    ty:A shower of rocks falls from the ceiling!
endif

// Oh no! A cave-in happens here! //
if cavein=20
  ty:^cYou are crushed by a cave-in!
  ty:
  ty:You are dead.
  ty:^h
  end
endif

