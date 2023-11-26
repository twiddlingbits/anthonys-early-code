   case NOUN:
    if (comcase != 1)
      comcase = 2;
      i = 1;
      while (code[rk + i] > 0)  {
         if (code[rk + i] == noun && comcase != 1)
            comcase = 0;
         i++;
         }
      comlen = i+1;
      break;
