
enum magazineinfo
{
	magazineValid,
	magazineName[64],
	Float:magazineX,
	Float:magazineY,
	Float:magazineZ,
	magazineInt,
}
new MagazineInfo[MAX_MAGAZINE][magazineinfo];
new Text3D:MagazineLabel[MAX_MAGAZINE];
new MagazinePickup[MAX_MAGAZINE];
new MagazineIcon[MAX_MAGAZINE];
stock LoadMagazine(i)
{
	new query[255];
	format(query,sizeof(query),"SELECT magazineName, magazineX, magazineY, magazineZ, magazineInt FROM magazines WHERE id=%d",i);
	mysql_tquery(MySQL, query, "SQL_LoadMagazine", "d", i);
	return 1;
}
forward SQL_LoadMagazine(i);
public SQL_LoadMagazine(i)
{
	new rows;
	cache_get_row_count(rows);
	if(rows>0)
	{
		//sscanf(Field,"p<|>s[64]fffi",
		cache_get_value_name(0,"magazineName",MagazineInfo[i][magazineName],64);
		cache_get_value_name_float(0,"magazineX",MagazineInfo[i][magazineX]);
		cache_get_value_name_float(0,"magazineY",MagazineInfo[i][magazineY]);
		cache_get_value_name_float(0,"magazineZ",MagazineInfo[i][magazineZ]);
		cache_get_value_name_int(0,"magazineInt",MagazineInfo[i][magazineInt]);
	    MagazineInfo[i][magazineValid]=1;
		MagazineLabel[i]=CreateDynamic3DTextLabel(MagazineInfo[i][magazineName], COLOR_GREEN, MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ], 15.0);
		MagazinePickup[i]=CreateDynamicPickup(19135,2,MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ]);
        MagazineIcon[i]=CreateDynamicMapIcon(MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ],38, -1,-1,-1, -1, 250.0);
	}
	else
	    MagazineInfo[i][magazineValid]=0;
	return 1;
}
stock MagazineInit()
{
    for(new i=0; i<MAX_MAGAZINE; i++)
        LoadMagazine(i);
	return 1;
}
stock SaveMagazine(i)
{
    if(MagazineInfo[i][magazineValid])
	{
	    new query[255];
	    format(query,sizeof(query),"UPDATE magazines SET magazineName='%s', magazineX='%f', magazineY='%f', magazineZ='%f', magazineInt=%d WHERE id=%d",
		MagazineInfo[i][magazineName],
		MagazineInfo[i][magazineX],
		MagazineInfo[i][magazineY],
		MagazineInfo[i][magazineZ],
		MagazineInfo[i][magazineInt],
		i);
		mysql_query(MySQL, query, false);
		DestroyDynamic3DTextLabel(MagazineLabel[i]);
		MagazineLabel[i]=CreateDynamic3DTextLabel(MagazineInfo[i][magazineName], COLOR_GREEN, MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ], 15.0);
		DestroyDynamicPickup(MagazinePickup[i]);
		MagazinePickup[i]=CreateDynamicPickup(19135,2,MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ]);
		DestroyDynamicMapIcon(MagazineIcon[i]);
        MagazineIcon[i]=CreateDynamicMapIcon(MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ],38, -1,-1,-1, -1, 250.0);
	}
	return 1;
}

stock CreateMagazine(playerid,interior,name[])
{
    for(new i=0; i<MAX_MAGAZINE; i++)
    {
        if(!MagazineInfo[i][magazineValid])
		{
		    new query[255];
		    GetPlayerPos(playerid,MagazineInfo[i][magazineX],
			MagazineInfo[i][magazineY],
			MagazineInfo[i][magazineZ]);
	    	format(query,sizeof(query),"INSERT INTO magazines SET id=%d, magazineName='%s', magazineX='%f', magazineY='%f', magazineZ='%f', magazineInt=%d",
			i,
			name,
			MagazineInfo[i][magazineX],
			MagazineInfo[i][magazineY],
			MagazineInfo[i][magazineZ],
			interior);
			mysql_query(MySQL, query, false);
			LoadMagazine(i);
			return i;
		}
    }
	return 1;
}
enum magazineineriorinfo
{
	Float:miX,
	Float:miY,
	Float:miZ,
	miInt,
};
new MagazineInteriorInfo[][magazineineriorinfo]=
{
	{-25.884499,-185.868988,1003.549988,17}
};
COMMAND:magazinecreate(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new interior,name[64];
    if (sscanf(params, "is[64]",interior,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /magazinecreate [id ���������] [�������� ��������]");
	if(interior>=sizeof(MagazineInteriorInfo))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� ������������ �������� ��������.");
	new i= CreateMagazine(playerid,interior,name);
	new str[255];
	format(str,sizeof(str),"�� ������� ������� %d, ����� ��������� %d, �������� %s",i,interior,name);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}

COMMAND:magazineshow(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    ShowMagazine(playerid,0,29);
    return 1;
}

stock ShowMagazine(playerid,startid,endid)
{
	new str[2048],bit;
	for(new i=startid; i<MAX_MAGAZINE; i++)
    {
        if(MagazineInfo[i][magazineValid])
        {
            if(i>endid)break;
            format(str,sizeof(str),"%s%d|%s\n",
            str,
            i,
        	MagazineInfo[i][magazineName]);
		    bit=i;
        }
    }
    format(str,sizeof(str),"%s%d|next",str,bit);
	new strm[255];
	format(strm,sizeof(strm),"������ ��������� [%d - %d]",startid,endid);
    ShowPlayerDialog(playerid,DIALOG_MAGAZINELIST,DIALOG_STYLE_LIST,strm,str,"�����","�����");
	return 1;
}

stock GotoMagazine(playerid,i)
{
	SetPlayerPos(playerid,MagazineInfo[i][magazineX],
	MagazineInfo[i][magazineY],
	MagazineInfo[i][magazineZ]);
	new str[255];
	format(str,sizeof(str),"�� ���������������� � �������� %d (%s)",i,MagazineInfo[i][magazineName]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}

COMMAND:magazinedelete(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,str[255];
    if (sscanf(params, "i",i))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /magazinedelete [id ��������]");
    MagazineInfo[i][magazineValid]=0;
    DestroyDynamic3DTextLabel(MagazineLabel[i]);
	DestroyDynamicPickup(MagazinePickup[i]);
	DestroyDynamicMapIcon(MagazineIcon[i]);
    format(str,sizeof(str),"DELETE FROM magazines WHERE id=%d",i);
    mysql_query(MySQL, str, false);
	format(str,sizeof(str),"�� ������� ������� %d",i);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:magazinesetname(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,name[64];
    if (sscanf(params, "is[64]",i,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /magazinesetname [id ��������] [�������� ��������]");
    format(MagazineInfo[i][magazineName],64,"%s",name);
	SaveMagazine(i);
    new str[255];
	format(str,sizeof(str),"�� �������� �������� �������� %d, �� %s",i,MagazineInfo[i][magazineName]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
    return 1;
}
COMMAND:magazinesetint(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,interior;
    if (sscanf(params, "is[64]",i,interior))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /magazinesetint [id ��������] [��������]");
    if(interior>=sizeof(MagazineInteriorInfo))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� ������������ �������� ��������.");
    MagazineInfo[i][magazineInt]=interior;
	SaveMagazine(i);
    new str[255];
	format(str,sizeof(str),"�� �������� �������� �������� %d, �� %s",i,MagazineInfo[i][magazineInt]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
    return 1;
}


COMMAND:buy(playerid,params[])
{
    if(PlayerInfo[playerid][pLocal]!=3)
        return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ��������.");
	new str[512],strm[6][64];
	new world=GetPlayerVirtualWorld(playerid);
	//new i = MagazineInfo[world][magazineInt];
	if(PlayerInfo[playerid][pCarRadarDetector])
		strm[0]="��� ����";
	else
		strm[0]="��� ���";
	if(PlayerInfo[playerid][pCarFuelTank])
		strm[1]="��� ����";
	else
		strm[1]="��� ���";
	if(PlayerInfo[playerid][pCarRadio])
		strm[2]="��� ����";
	else
		strm[2]="��� ���";
	if(PlayerInfo[playerid][pCarAdditive])
	    strm[3]="��� ����";
	else
		strm[3]="��� ���";
	if(PlayerInfo[playerid][pCarNeck])
	    strm[4]="��� ����";
	else
		strm[4]="��� ���";
	if(PlayerInfo[playerid][pCarDamper])
		strm[5]="��� ����";
	else
	    strm[5]="��� ���";
	format(str,sizeof(str),
	"[%s]����� ��������[10000$]\n[%s]�������������� ��������[20000$]\n[%s]������������ �����(������� ������ ��������)[7000$]\n[%s]�������� � ��������� [12000$]\n[%s]���������� ��������� ��������� [15000$]\n[%s]�������������� ��������� ���� [30000$]",
	strm[0],
	strm[1],
	strm[2],
	strm[3],
	strm[4],
	strm[5]);
    ShowPlayerDialog(playerid,DIALOG_BUY,DIALOG_STYLE_LIST,MagazineInfo[world][magazineName],str,"�������","������");
	return 1;
}

stock PlayerInMagazine(playerid,listitem)
{
	switch(listitem)
	{
	    case 0://�����
		{
		    if(oGetPlayerMoney(playerid)<10000)
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
		    oGivePlayerMoney(playerid,-10000);
		    PlayerInfo[playerid][pCarRadarDetector]=1;
		    Me(playerid, "����� �����-��������",20.0);
		}
		case 1://�������������� ��������
		{
		    if(oGetPlayerMoney(playerid)<20000)
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
		    oGivePlayerMoney(playerid,-20000);
		    PlayerInfo[playerid][pCarFuelTank]=1;
		    Me(playerid, "����� �������������� �������� ��� ������ ���������",20.0);
		}
		case 2://�����
		{
		    if(oGetPlayerMoney(playerid)<7000)
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
		    oGivePlayerMoney(playerid,-7000);
		    PlayerInfo[playerid][pCarRadio]=1;
		    Me(playerid, "����� ������������ ����� ��� ������ ���������",20.0);
		}
		case 3://��������
		{
		    if(oGetPlayerMoney(playerid)<12000)
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
		    oGivePlayerMoney(playerid,-12000);
		    PlayerInfo[playerid][pCarAdditive]=1;
		    Me(playerid, "����� �������� � ��������� ��� ������ ���������",20.0);
		}
		case 4://��������� ���������
		{
		    if(oGetPlayerMoney(playerid)<15000)
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
		    oGivePlayerMoney(playerid,-15000);
		    PlayerInfo[playerid][pCarNeck]=1;
		    Me(playerid, "����� ���������� ��������� ��������� ��� ������ ���������",20.0);
		}
		case 5:
		{
		    if(!PlayerInfo[playerid][pCarRadarDetector])
		        return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","������� ������ ����� ��������","��","");
			if(oGetPlayerMoney(playerid)<30000)
			    return ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"�������","� ��� ������������ �����","��","");
			oGivePlayerMoney(playerid,-30000);
			PlayerInfo[playerid][pCarDamper]=1;
			Me(playerid, "����� �������������� ��������� ���� ��� ������ ���������",20.0);
		}
	}
	return 1;
}

stock MagazineExitFor(playerid)
{
    for(new i=0; i<MAX_MAGAZINE; i++)
    {
        if(MagazineInfo[i][magazineValid])
        {
            if(IsPlayerInRangeOfPoint(playerid,2.0,MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ]))
            {
                PlayerInfo[playerid][pLocal]=3;
                SetPlayerPos(playerid,MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miX],
                MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miY],
                MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miZ]);
                SetPlayerInterior(playerid,MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miInt]);
                SetPlayerVirtualWorld(playerid,i);
                SendClientMessage(playerid,COLOR_WHITE," �����������: {FFFF00}/buy - {FFFFFF}����� ������ ���-����.");
                return 1;
            }
        }
    }
    return 0;
}

stock MagazineExitTimer(playerid)
{
    new i=GetPlayerVirtualWorld(playerid);
    if(IsPlayerInRangeOfPoint(playerid,2.0,MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miX],
    MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miY],
    MagazineInteriorInfo[MagazineInfo[i][magazineInt]][miZ]))
    {
        PlayerInfo[playerid][pLocal]=-1;
        SetPlayerPos(playerid,MagazineInfo[i][magazineX],MagazineInfo[i][magazineY],MagazineInfo[i][magazineZ]);
        SetPlayerInterior(playerid,0);
        SetPlayerVirtualWorld(playerid,0);
        return 1;
    }
    return 0;
}