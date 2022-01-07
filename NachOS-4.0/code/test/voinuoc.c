#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	int  fID_voinuoc, fID_result;	// Bien id cho file
	char c;	// Bien ki tu luu ki tu doc tu file
	int v1, v2;		// Voi 1, voi 2
	int v;			// Dung tich nuoc cua sinh vien
	int flag_done_result;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	v1 = v2 = 0;
	// Xu ly voi nuoc
	// WHILE(11111111111111111111111111111111111111)
	while(1)
	{
		Wait("m_vn");

		// Mo file result.txt de ghi voi nao su dung
		fID_result = Open("result.txt", 2);
		if(fID_result == -1)
		{		//?
			Signal("sinhvien");
			return;
		}
		// WHILE(2222222222222222222222222222222222222222)
		while(1)
		{
			Wait("voinuoc");
			c = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			fID_voinuoc = Open("voinuoc.txt", 3);
			// PrintString("Si voi nuoc cua voi nuoc: ");
			// PrintNum(fID_voinuoc);
			if(fID_voinuoc == -1)
			{
				//?
				Close(fID_result);
				Signal("sinhvien");
			
				return;
			}
		
			v = 0;
			flag_done_result = 0;
			// WHILE(3333333333333333333333333333333333333333333333)
			while(1)
			{			
				if(Read(&c, 1, fID_voinuoc)  == -2)
				{	
					Close(fID_voinuoc);			
					break;
				}
				if(c != '*')
				{
					v = v* 10 + (c - '0');
				}
				else
				{
					flag_done_result = 1;				
					Close(fID_voinuoc);
					break;			
				}
			
			}
			// WHILE(3333333333333333333333333333333333333333333333)
			//PrintNum(v);
			if(v!= 0)
			{
				// Dung voi 1
				if(v1 <= v2)
				{
					v1 += v;
					Write("1", 1, fID_result);
				}
				else	// Dung voi 2
				{					
					v2 += v;
					Write("2", 1, fID_result);
					
				}
			}
		
			if(flag_done_result == 1)
			{
				v1 = v2 = 0;
				Close(fID_result);				
				Signal("sinhvien");
				break;				
			}

			Signal("sinhvien");
		}
		// WHILE(2222222222222222222222222222222222222222)	
	}
	// WHILE(11111111111111111111111111111111111111)			
}
