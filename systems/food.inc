enum foodinfo
{
	foodValid,
	foodName[64],
	Float:foodEnX,
	Float:foodEnY,
	Float:foodEnZ,
	foodInt,
};
new FoodInfo[MAX_FOOD][foodinfo];
new Text3D:FoodLabel[MAX_FOOD];
new FoodPickup[MAX_FOOD];
new FoodIcon[MAX_FOOD];

stock LoadFood(i)
{
    new query[255];
	format(query,sizeof(query),"SELECT foodName, foodEnX, foodEnY, foodEnZ, foodInt FROM foods WHERE id=%d",i);
	mysql_tquery(MySQL, query, "SQL_LoadFood", "d", i);
	return 1;
}
forward SQL_LoadFood(i);
public SQL_LoadFood(i)
{
	new rows;
	cache_get_row_count(rows);
	if(rows>0)
	{
		//sscanf(Field,"p<|>s[64]fffd",
		cache_get_value_name(0,"foodName",FoodInfo[i][foodName],64);
		cache_get_value_name_float(0,"foodEnX",FoodInfo[i][foodEnX]);
		cache_get_value_name_float(0,"foodEnY",FoodInfo[i][foodEnY]);
		cache_get_value_name_float(0,"foodEnZ",FoodInfo[i][foodEnZ]);
		cache_get_value_name_int(0,"foodInt",FoodInfo[i][foodInt]);
	    FoodInfo[i][foodValid]=1;
	    FoodLabel[i]=CreateDynamic3DTextLabel(FoodInfo[i][foodName], COLOR_GREEN, FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ], 15.0);
		FoodPickup[i]=CreateDynamicPickup(19135,2,FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ]);
		FoodIcon[i]=CreateDynamicMapIcon(FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ],50, -1,-1,-1, -1, 250.0);
 	}
 	else
 	    FoodInfo[i][foodValid]=0;
	return 1;
}
stock FoodInit()
{
    for(new i=0; i<MAX_FOOD; i++)
        LoadFood(i);
	return 1;
}
stock SaveFood(i)
{
    if(FoodInfo[i][foodValid])
	{
	    new query[255];
	    format(query,sizeof(query),"UPDATE foods SET foodName='%s', foodEnX='%f', foodEnY='%f', foodEnZ='%f', foodInt=%d WHERE id=%d",
	    FoodInfo[i][foodName],
		FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ],
		FoodInfo[i][foodInt],
		i);
		mysql_query(MySQL, query, false);
		DestroyDynamic3DTextLabel(FoodLabel[i]);
		FoodLabel[i]=CreateDynamic3DTextLabel(HotelInfo[i][hotelName], COLOR_GREEN, FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ], 15.0);
		DestroyDynamicPickup(FoodPickup[i]);
		FoodPickup[i]=CreateDynamicPickup(19135,2,FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ]);
		DestroyDynamicMapIcon(FoodIcon[i]);
		FoodIcon[i]=CreateDynamicMapIcon(FoodInfo[i][foodEnX],
		FoodInfo[i][foodEnY],
		FoodInfo[i][foodEnZ],50, -1,-1,-1, -1, 250.0);
	}
	return 1;
}
stock CreateFood(playerid,interior,name[])
{
    for(new i=0; i<MAX_FOOD; i++)
    {
        if(!FoodInfo[i][foodValid])
		{
		    new query[255];
		    GetPlayerPos(playerid,FoodInfo[i][foodEnX],FoodInfo[i][foodEnY],FoodInfo[i][foodEnZ]);
	    	format(query,sizeof(query),"INSERT INTO foods SET id=%d, foodName=' %s', foodEnX='%f', foodEnY='%f', foodEnZ='%f', foodInt=%d",
			i,
			name,
			FoodInfo[i][foodEnX],
			FoodInfo[i][foodEnY],
			FoodInfo[i][foodEnZ],
			interior);
			mysql_query(MySQL, query, false);
			LoadFood(i);
			return i;
		}
    }
	return 1;
}
enum foodineriorinfo
{
	Float:fiX,
	Float:fiY,
	Float:fiZ,
	fiInt,
};
new FoodInteriorInfo[][foodineriorinfo]=
{
	{363.10000610352,-75.0,1001.5,10},//Burger Shot
	{372.39999389648,-133.30000305176,1001.5,5},//Well Stacked Pizza
	{364.89999389648,-11.5,1001.9000244141,9},//cluckin'bell
	{377.10000610352,-193.0,1000.5999755859,17}//Rusty Brown's Donuts
};
COMMAND:foodcreate(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new interior,name[64];
    if (sscanf(params, "is[64]",interior,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /foodcreate [id ���������] [�������� �����������]");
	if(interior>=sizeof(FoodInteriorInfo))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� ������������ �������� �����������.");
	new i= CreateFood(playerid,interior,name);
	new str[255];
	format(str,sizeof(str),"�� ������� ����������� %d, ����� ��������� %d, �������� %s",i,interior,name);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}

COMMAND:foodshow(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    ShowFood(playerid,0,29);
    return 1;
}

stock ShowFood(playerid,startid,endid)
{
	new str[2048],bit;
	for(new i=startid; i<MAX_FOOD; i++)
    {
        if(FoodInfo[i][foodValid])
        {
            if(i>endid)break;
            format(str,sizeof(str),"%s%d|%s\n",
            str,
            i,
        	FoodInfo[i][foodName]);
		    bit=i;
        }
    }
    format(str,sizeof(str),"%s%d|next",str,bit);
	new strm[255];
	format(strm,sizeof(strm),"������ ����������� [%d - %d]",startid,endid);
    ShowPlayerDialog(playerid,DIALOG_FOODLIST,DIALOG_STYLE_LIST,strm,str,"�����","�����");
	return 1;
}

stock GotoFood(playerid,i)
{
	SetPlayerPos(playerid,FoodInfo[i][foodEnX],FoodInfo[i][foodEnY],FoodInfo[i][foodEnZ]);
	new str[255];
	format(str,sizeof(str),"�� ���������������� � ����������� %d (%s)",i,FoodInfo[i][foodName]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}

COMMAND:fooddelete(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,str[255];
    if (sscanf(params, "i",i))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /fooddelete [id �����������]");
    FoodInfo[i][foodValid]=0;
    DestroyDynamic3DTextLabel(FoodLabel[i]);
	DestroyDynamicPickup(FoodPickup[i]);
	DestroyDynamicMapIcon(FoodIcon[i]);
    format(str,sizeof(str),"DELETE FROM foods WHERE id=%d",i);
    mysql_query(MySQL, str, false);
	format(str,sizeof(str),"�� ������� ����������� %d",i);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:foodsetname(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,name[64];
    if (sscanf(params, "is[64]",i,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /foodsetname [id �����������] [�������� �����������]");
    format(FoodInfo[i][foodName],64," %s",name);
	SaveFood(i);
    new str[255];
	format(str,sizeof(str),"�� �������� �������� ����������� %d, �� %s",i,FoodInfo[i][foodName]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
    return 1;
}
COMMAND:foodsetint(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i,interior;
    if (sscanf(params, "ii",i,interior))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /foodsetint [id �����������] [��������]");
    if(interior>=sizeof(FoodInteriorInfo))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� ������������ �������� �����������.");
    FoodInfo[i][foodInt]=interior;
	SaveFood(i);
    new str[255];
	format(str,sizeof(str),"�� �������� �������� ����������� %d, �� %d",i,FoodInfo[i][foodInt]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
    return 1;
}

enum eat
{
	eatName[64],
	eatPrice,
	Float:eatHunger
}
new Eat[sizeof(FoodInteriorInfo)][5][eat]=
{
{{"��� ����",4,25.0},{"������",2,10.0},{"���������",3,17.0},{"���� ����",2,7.0},{"��������",6,18.0}},
{{"������������ �����",6,50.0},{"����� 4 ����",5,40.0},{"����� ���������",5,40.0},{"���������� ����",2,7.0},{"����-���������",7,21.0}},
{{"������ ��������",5,30.0},{"������ � �������",4,25.0},{"�������� ���",4,30.0},{"������",2,7.0},{"����",6,20.0}},
{{"������� � �������� ������",3,20.0},{"������� � ���������� �������",3,23.0},{"��������� ��������",2,12.0},{"���",2,7.0},{"��������",12,30.0}}
//�����,��������� ���,�������,����
};
COMMAND:food(playerid,params[])
{
    if(PlayerInfo[playerid][pLocal]!=2)
        return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � �����������.");
	new str[255];
	new world=GetPlayerVirtualWorld(playerid);
	new i = FoodInfo[world][foodInt];
	format(str,sizeof(str),"%s [%d$]\n%s [%d$]\n%s [%d$]\n%s [%d$]\n%s [%d$]",
	Eat[i][0][eatName],
	Eat[i][0][eatPrice],
	Eat[i][1][eatName],
	Eat[i][1][eatPrice],
	Eat[i][2][eatName],
	Eat[i][2][eatPrice],
	Eat[i][3][eatName],
	Eat[i][3][eatPrice],
	Eat[i][4][eatName],
	Eat[i][4][eatPrice]);
    ShowPlayerDialog(playerid,DIALOG_FOOD,DIALOG_STYLE_LIST,FoodInfo[world][foodName],str,"�������","������");
	return 1;
}
stock PlayerEat(playerid,listitem)
{
    new world=GetPlayerVirtualWorld(playerid);
	new i = FoodInfo[world][foodInt];
	if(listitem==4)
	{
	    if(PlayerCoffee[playerid] > 2)
	        return SendClientMessage(playerid,COLOR_GRAD2,"������ ������ ���� ����, ��� ������.");
	    PlayerCoffee[playerid]++;
	    PlayerInfo[playerid][pFatigue]-=Eat[i][4][eatHunger];
	    if(PlayerInfo[playerid][pFatigue]<0.0)
            PlayerInfo[playerid][pFatigue]=0.0;
	}
	else
	{
		PlayerInfo[playerid][pHunger]-=Eat[i][listitem][eatHunger];//�����
		if(PlayerInfo[playerid][pHunger]<0.0)
            PlayerInfo[playerid][pHunger]=0.0;
	}
	new str[255];
	format(str,sizeof(str),"�� ���������� %s",Eat[i][listitem][eatName]);
	ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,FoodInfo[world][foodName],str,"��","");
	oGivePlayerMoney(playerid, -Eat[i][listitem][eatPrice]);
	return 1;
}
stock FoodExitFor(playerid)
{
    for(new i=0; i<MAX_FOOD; i++)
        {
            if(FoodInfo[i][foodValid])
            {
                if(IsPlayerInRangeOfPoint(playerid,2.0,FoodInfo[i][foodEnX],FoodInfo[i][foodEnY],FoodInfo[i][foodEnZ]))
                {
                    PlayerInfo[playerid][pLocal]=2;
                    SetPlayerPos(playerid,FoodInteriorInfo[FoodInfo[i][foodInt]][fiX],
                    FoodInteriorInfo[FoodInfo[i][foodInt]][fiY],
                    FoodInteriorInfo[FoodInfo[i][foodInt]][fiZ]);
                    SetPlayerInterior(playerid,FoodInteriorInfo[FoodInfo[i][foodInt]][fiInt]);
                    SetPlayerVirtualWorld(playerid,i);
                    SendClientMessage(playerid,COLOR_WHITE," �����������: {FFFF00}/food - {FFFFFF}����� ������ ���-���� ������.");
                    return 1;
                }
            }
        }
    return 0;
}
stock FoodExitTimer(playerid)
{
    new i=GetPlayerVirtualWorld(playerid);
    if(IsPlayerInRangeOfPoint(playerid,2.0,FoodInteriorInfo[FoodInfo[i][foodInt]][fiX],
    FoodInteriorInfo[FoodInfo[i][foodInt]][fiY],
    FoodInteriorInfo[FoodInfo[i][foodInt]][fiZ]))
    {
        PlayerInfo[playerid][pLocal]=-1;
        SetPlayerPos(playerid,FoodInfo[i][foodEnX],FoodInfo[i][foodEnY],FoodInfo[i][foodEnZ]);
        SetPlayerInterior(playerid,0);
        SetPlayerVirtualWorld(playerid,0);
        return 1;
    }
    return 0;
}