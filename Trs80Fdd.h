// Trs80Fdd.h: interface for the CTrs80Fdd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TRS80FDD_H)
#define TRS80FDD_H

#include "Trs80Configurator.h"

//
// BASE Class
//
class CTrs80Device
{
public:
	CTrs80Device();
	virtual ~CTrs80Device();

	virtual short CanWrZ80(unsigned short, unsigned char, CTrs80Configurator* );
	virtual short CanRdZ80(unsigned short, unsigned char *);
	virtual short ClockTick(int newclocks);
};

#define TRS80_DEVRET_UNHANDLED		0
#define TRS80_DEVRET_HANDLED		1
#define TRS80_DEVRET_TRIG_INT		2
#define TRS80_DEVRET_CLEAR_INT		3

//
// Floppy Disk Base Class

class CTrs80Fdd : public CTrs80Device 
{
public:
	CTrs80Fdd();
	virtual ~CTrs80Fdd();

	virtual void LoadDos(short);
	virtual void LoadDisk(short drive_num, unsigned char* dsk);
};


//
// Floppy Implementation Class
//

#define FDD_MAX_DRIVES 8

class CTrs80FddSimple : public CTrs80Fdd 
{
public:
	CTrs80FddSimple();
	virtual ~CTrs80FddSimple();

	virtual short CanWrZ80(register unsigned short Addr, register unsigned char Value, CTrs80Configurator*);
	virtual short CanRdZ80(register unsigned short Addr, unsigned char* retvalue);
	virtual short ClockTick(int newclocks);
	virtual void  LoadDos(short dos_type);
	virtual void  LoadDisk(short drive_num, unsigned char* dsk);

public:
	unsigned char* m_disk_image[FDD_MAX_DRIVES];	

private:
	unsigned char m_fdd_sector_reg;		
	unsigned char m_fdd_track_reg;		
	unsigned char m_fdd_status_reg;
	unsigned char m_fdd_data_reg;
	unsigned char m_fdd_state;
	unsigned short m_cur_secoffset;
	unsigned short m_drive_select;
	__int64 m_fdd_timeout_clocks;		// Drive motor time out count

};


//
// fdd state
//
#define FDD_UNINIT		0

#define FDD_TYPE_I		1
#define FDD_TYPE_II		2

#define FDD_RESTORE		(FDD_TYPE_I |  0x10)
#define FDD_SEEK		(FDD_TYPE_I |  0x20)
#define FDD_STEP_IN		(FDD_TYPE_I |  0x30)
#define FDD_STEP_OUT	(FDD_TYPE_I |  0x40)
#define FDD_FORCE		(FDD_TYPE_I |  0x50)		// acts as type I for purposes of status reg
#define FDD_READ_SEC	(FDD_TYPE_II | 0x60)
#define FDD_WRITE_SEC	(FDD_TYPE_II | 0x70)

// Status bits after Type I command from WDD Data sheet
#define FDD_STATUS_NOT_READY	(1<<7)
#define FDD_STATUS_WR_PROTECT	(1<<6)
#define FDD_STATUS_HEAD_LOADED	(1<<5)	// TRS80 books says this is 0
#define FDD_STATUS_SEEK_ERROR	(1<<4)
#define FDD_STATUS_CRC_ERROR	(1<<3)
#define FDD_STATUS_TRACK_ZERO	(1<<2)   // TRS80 book says this is "MISSING ADDRESS"
#define FDD_STATUS_INDEX_PULSE	(1<<1)
#define FDD_STATUS_BUSY			(1<<0)

// Status bits after type 2 from WDD Data sheet
#define FDD_STATUS_DDM			(1<<5)		// Delete data mark record type
#define FDD_STATUS_LOST_DATA	(1<<2)
#define FDD_STATUS_DRQ			(1<<1)


#endif // !defined(TRS80FDD_H)
