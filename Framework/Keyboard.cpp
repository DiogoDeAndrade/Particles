// Keyboard.cpp: implementation of the Keyboard class.
//
//////////////////////////////////////////////////////////////////////

#include "Keyboard.h"
#include "Utils.h"
#include "Gfx.h"

Keyboard*		KEY=NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Keyboard::Keyboard()
{
	if (!KEY) KEY=this;
	_keyboard=NULL;
}

Keyboard::~Keyboard()
{
	if (KEY==this) KEY=NULL;
	if (_keyboard) _keyboard->Release();
	if (LPDI) 
	{
		if (LPDI->Release()==0) LPDI=NULL;
	}
}

bool Keyboard::init(HINSTANCE instance)
{
	if (!LPDI)
	{
		HRESULT hRet;

		hRet=DirectInput8Create(instance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&LPDI,NULL);
		if (hRet!=DI_OK)
		{
			ERROR_MSG("Couldn't initialize DirectInput!");
			return false;
		}
	}

	HRESULT hRet=LPDI->CreateDevice(GUID_SysKeyboard,&_keyboard,NULL);
	if (hRet!=DI_OK)
	{
		ERROR_MSG("Can't initialize keyboard!");
		if (LPDI) LPDI->Release(); LPDI=NULL;	
		return false;
	}
	
	hRet=_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (hRet!=DI_OK)
	{
		_keyboard->Release();
		if (LPDI) LPDI->Release(); LPDI=NULL;	
		ERROR_MSG("Couldn't set the data format for the keyboard!");
		return false;
	}

	hRet=_keyboard->SetCooperativeLevel(GFX->get_window(),DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	if (hRet!=DI_OK)
	{
		ERROR_MSG("Couldn't set cooperative level for the keyboard!");
	}

	hRet=_keyboard->Acquire();
	if (hRet!=DI_OK)
	{
		ERROR_MSG("Couldn't acquire the keyboard!");
	}

	return true;
}

void Keyboard::poll()
{
	unsigned char buffer[256];
	
	HRESULT hRet=_keyboard->Poll();

	hRet=_keyboard->GetDeviceState(sizeof(buffer),(void*)buffer); 
	if (hRet!=DI_OK)
	{
		switch (hRet)
		{
			case DIERR_INPUTLOST:
			case DIERR_NOTACQUIRED:
				hRet=_keyboard->Acquire();
				if (hRet!=DI_OK)
				{
					ERROR_MSG("Couldn't poll keyboard: Couldn't acquire the keyboard!");
					return;
				}
				else
				{
					hRet=_keyboard->GetDeviceState(sizeof(buffer),(void*)&buffer); 
					if (hRet!=DI_OK)
					{
						ERROR_MSG("Couldn't poll keyboard: Input lost!");
						return;
					}
				}
				break;
			case DIERR_INVALIDPARAM:
				ERROR_MSG("Couldn't poll keyboard: Invalid parameter!");
				return;
				break;
			case DIERR_NOTINITIALIZED:
				ERROR_MSG("Couldn't poll keyboard: Not initialized!");
				return;
				break;
			case E_PENDING:
				ERROR_MSG("Couldn't poll keyboard: Data is not yet available!");
				return;
				break;
		}
	}

	memcpy(_last_state,buffer,256);
}

int	 Keyboard::any_pressed()
{
	for (int i=0; i<256; i++)
	{
		if (_last_state[i] & 0x80) return 1;
	}
	return 0;
}
