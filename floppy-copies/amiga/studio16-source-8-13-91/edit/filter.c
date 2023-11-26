
void filter_on()

{
unsigned char *ptr;
unsigned char val;

ptr = 0xbfe001;

val = *ptr;

*ptr = val & 0xFD;
}

void filter_off()

{
unsigned char *ptr;
unsigned char val;

ptr = 0xbfe001;

val = *ptr;

*ptr = val | 2;
}

