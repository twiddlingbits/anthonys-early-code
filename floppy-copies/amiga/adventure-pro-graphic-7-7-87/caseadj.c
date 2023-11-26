   case ADJ:
    if (comcase != 1 && comcase != 2)
      comcase = 3;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == adj && comcase != 1 && comcase != 2)
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;
