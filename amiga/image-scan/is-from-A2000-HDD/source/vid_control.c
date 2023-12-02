
static unsigned char *data_dir,*data_dir2,*data,*data2;


void init_scanner()

{
data     = (unsigned char *)0xBFE101;
data_dir = (unsigned char *)0xBFE301;
data2    = (unsigned char *)0xBFD000;
data_dir2= (unsigned char *)0xBFD200;
}

void select_scanner()
{
*data_dir=0;   /** Data lines are all inputs now **/
*data_dir2=2;  /** POUT only is write **/
*data2=0;      /** Select Scanner **/
}

void select_printer()
{
*data_dir=0xFF;   /** Data lines are all outputs now **/
*data2=0;         /** Select printer **/
*data_dir2=0;     /** POUT is read **/
}

get_byte()
{
unsigned char val;

select_scanner();
val = *data;
select_printer();

return(val);
}

void send_byte(val)
unsigned char val;
{
while (*data2&1) ;      /** wait for busy to stop **/
*data = val;
}

