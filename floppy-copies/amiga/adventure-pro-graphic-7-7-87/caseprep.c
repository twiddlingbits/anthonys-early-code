   case PREP:
    if (comcase <1 || comcase > 3)
      comcase = 4;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == prepobj && (comcase < 1 || comcase > 3 ))
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;
