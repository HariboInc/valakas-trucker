
enum deliveryinfo
{
    deliveryValid,
    deliveryProd[32],
    deliveryStartProp,
    deliveryEndProp,
    deliveryFragility,
    deliveryPrice,
    deliveryTrailer,
	Float:deliveryValue,
};
new DeliveryInfo[MAX_DELIVERYS][deliveryinfo];
stock LoadDelivery(i)
{
    new query[255];
	format(query,sizeof(query),"SELECT deliveryProd, deliveryStartProp, deliveryEndProp, deliveryFragility, deliveryPrice, deliveryTrailer, deliveryValue FROM deliverys WHERE id=%d",i);
	mysql_tquery(MySQL, query, "SQL_LoadDelivery", "d", i);
	return 1;
}
forward SQL_LoadDelivery(i);
public SQL_LoadDelivery(i)
{
	new rows;
	cache_get_row_count(rows);
	if(rows>0)
	{
		//sscanf(Field,"p<|>s[32]dddddf",
		cache_get_value_name(0,"deliveryProd",DeliveryInfo[i][deliveryProd],64);
	    cache_get_value_name_int(0,"deliveryStartProp",DeliveryInfo[i][deliveryStartProp]);
	   	cache_get_value_name_int(0,"deliveryEndProp", DeliveryInfo[i][deliveryEndProp]);
	    cache_get_value_name_int(0,"deliveryFragility",DeliveryInfo[i][deliveryFragility]);
	    cache_get_value_name_int(0,"deliveryPrice",DeliveryInfo[i][deliveryPrice]);
		cache_get_value_name_int(0,"deliveryTrailer",DeliveryInfo[i][deliveryTrailer]);
		cache_get_value_name_float(0,"deliveryValue",DeliveryInfo[i][deliveryValue]);
	    DeliveryInfo[i][deliveryValid]=1;
	}
	else
	    DeliveryInfo[i][deliveryValid]=0;
	return 1;
}
stock DeliveryInit()
{
    for(new i=0; i<MAX_DELIVERYS; i++)
        LoadDelivery(i);
	return 1;
}
stock SaveDelivery(i)
{
    if(DeliveryInfo[i][deliveryValid])
	{
	    new query[255];
	    format(query,sizeof(query),"UPDATE deliverys SET deliveryProd='%s', deliveryStartProp=%d, deliveryEndProp=%d, deliveryFragility=%d, deliveryPrice=%d, deliveryValue='%f' WHERE id=%d",
		DeliveryInfo[i][deliveryProd],
	    DeliveryInfo[i][deliveryStartProp],
	    DeliveryInfo[i][deliveryEndProp],
	    DeliveryInfo[i][deliveryFragility],
	    DeliveryInfo[i][deliveryPrice],
	    DeliveryInfo[i][deliveryTrailer],
	    DeliveryInfo[i][deliveryValue],
		i);
		mysql_query(MySQL, query, false);
	}
	return 1;
}

stock CreateDelivery(name[],start,end,fragility,price,trailer,Float:value)
{
    for(new i=0; i<MAX_DELIVERYS; i++)
    {
        if(!DeliveryInfo[i][deliveryValid])
        {
            new query[255];
	        format(query,sizeof(query),"INSERT INTO deliverys SET deliveryProd=' %s', deliveryStartProp=%d, deliveryEndProp=%d, deliveryFragility=%d, deliveryPrice=%d, deliveryTrailer=%d, deliveryValue='%f', id=%d",
			name,start,end,fragility,price,trailer,value,
			i);
			mysql_query(MySQL, query, false);
	        LoadDelivery(i);
	        return i;
        }
    }
    return 1;
}

COMMAND:deliverycreate(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new name[32],start,price,end,fragility,trailer,Float:value;
    if (sscanf(params, "fiiiis[32]",value,start,end,fragility,trailer,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverycreate [����.�������� 0.1-0.9] [��������� ���] [�������� ���] [���������(10-90)] [��� ��������] [�-�� ���-��]");
	if(value < 0.1 || value > 0.9)
	    return SendClientMessage(playerid, COLOR_GRAD2, "����������� �������� �� 0.1 �� 0.9");
	if(start==end)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� ����� ������� �� ����� ��������� � ���������");
	if(!PropInfo[start][propValid])
	    return SendClientMessage(playerid, COLOR_GRAD2, "��������� ����� �� ����������");
	if(!PropInfo[end][propValid])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� ����� �� ����������");
	if(fragility < 10 || fragility > 90)
	    return SendClientMessage(playerid, COLOR_GRAD2, "��������� �� 10 �� 90");
	if(trailer < 0 || trailer > 3)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� 0 - ������� � ����������� �������, 1 - �������, 2 - ��������, 3 - ��������� ���������");
    new Float:floatprice=GetDistanceBetweenPoints(PropInfo[start][propX],PropInfo[start][propY],PropInfo[start][propZ],PropInfo[end][propX],PropInfo[end][propY],PropInfo[end][propZ]);
    new str[255];
	format(str,sizeof(str),"%f",floatprice);
	price=strval(str);
	new i=CreateDelivery(name,start,end,fragility,price,trailer,value);
	new str1[32];
	switch(trailer)
	{
	    case 0:str1="�������";
	    case 1:str1="�����";
	    case 2:str1="��������";
	    case 3:str1="��������� ��������";
	}
	format(str,sizeof(str),"�� ������� ��������, ���� [%s], ��������� [%d], ���� [%d], ��� �������[%s] ����������� �������� [%f]",
	DeliveryInfo[i][deliveryProd],
    DeliveryInfo[i][deliveryFragility],
    DeliveryInfo[i][deliveryPrice],
	str1,
	DeliveryInfo[i][deliveryValue]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	format(str,sizeof(str),"����� [%s (%d)], ����� [%s (%d)]",
	PropInfo[DeliveryInfo[i][deliveryStartProp]][propName],
    DeliveryInfo[i][deliveryStartProp],
    PropInfo[DeliveryInfo[i][deliveryEndProp]][propName],
    DeliveryInfo[i][deliveryEndProp]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverydelete(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i;
    if (sscanf(params, "i",i))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverydelete [�� ��������]");
    if(!DeliveryInfo[i][deliveryValid])
        return SendClientMessage(playerid, COLOR_GRAD2, "������ �������.");
	new query[255];
	format(query,sizeof(query),"DELETE FROM deliveys WHERE id=%d",i);
	mysql_query(MySQL, query, false);
	LoadDelivery(i);
    format(query,sizeof(query),"�� ������� �������� %d",i);
    SendClientMessage(playerid, COLOR_GRAD2, query);
	return 1;
}
COMMAND:propdelete(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new i;
    if (sscanf(params, "i",i))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /propdelete [�� ��������]");
    if(!PropInfo[i][propValid])
        return SendClientMessage(playerid, COLOR_GRAD2, "������ �������.");
	new query[255];
	format(query,sizeof(query),"DELETE FROM props WHERE id=%d",i);
	mysql_query(MySQL, query, false);
	LoadProp(i);
    format(query,sizeof(query),"�� ������� ������ %d",i);
    SendClientMessage(playerid, COLOR_GRAD2, query);
	return 1;
}
COMMAND:deliverysetprod(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new name[32],i;
    if (sscanf(params, "is[32]",i,name))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetprod [id ��������] [�������� ��������]");
	format(DeliveryInfo[i][deliveryProd],32," %s",name);
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ��� �������� %d - %s",i,name);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverysetstart(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new start,i;
    if (sscanf(params, "ii",i,start))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetstart [id ��������] [id ���������� ��������]");
    if(!PropInfo[start][propValid])
	    return SendClientMessage(playerid, COLOR_GRAD2, "��������� ����� �� ����������");
	DeliveryInfo[i][deliveryStartProp]=start;
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ����� �������� %d - %s (%d)",i,PropInfo[DeliveryInfo[i][deliveryStartProp]][propName],DeliveryInfo[i][deliveryStartProp]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverysetend(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new start,i;
    if (sscanf(params, "ii",i,start))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetend [id ��������] [id ��������� ��������]");
    if(!PropInfo[start][propValid])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� ����� �� ����������");
	DeliveryInfo[i][deliveryEndProp]=start;
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ����� �������� %d - %s (%d)",i,PropInfo[DeliveryInfo[i][deliveryEndProp]][propName],DeliveryInfo[i][deliveryEndProp]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverysetfragility(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new fragility,i;
    if (sscanf(params, "ii",i,fragility))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetfragility [id ��������] [��������� 0-100]");
    if(fragility < 0 || fragility > 100)
	    return SendClientMessage(playerid, COLOR_GRAD2, "��������� �� 0 �� 100");
	DeliveryInfo[i][deliveryFragility]=fragility;
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ��������� �������� %d - %d",i,DeliveryInfo[i][deliveryFragility]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverysetprice(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new fragility,i;
    if (sscanf(params, "ii",i,fragility))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetprice [id ��������] [����]");
	DeliveryInfo[i][deliveryPrice]=fragility;
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ���� �������� %d - %d",i,DeliveryInfo[i][deliveryPrice]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliverysettrailer(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    new trailer,i;
    if (sscanf(params, "ii",i,trailer))
    	return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /deliverysetprice [id ��������] [��� ��������]");
    if(trailer < 0 || trailer > 2)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� 0 - ������� � ����������� �������, 1 - �������, 2 - ��������.");
    new str1[24];
	switch(trailer)
	{
	    case 0:str1="�������";
	    case 1:str1="�����";
	    case 2:str1="��������";
	}
	DeliveryInfo[i][deliveryTrailer]=trailer;
	SaveDelivery(i);
	new str[255];
	format(str,sizeof(str),"�� ���������� ������� �������� %d - %s (%d)",i,str1,DeliveryInfo[i][deliveryTrailer]);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:deliveryshow(playerid,params[])
{
    if(PlayerInfo[playerid][pAdmin]<1)return 1;
    ShowDelivery(playerid,0,29);
    return 1;
}
stock ShowDelivery(playerid,startid,endid)
{
	new str[4096],bit;
	for(new i=startid; i<MAX_DELIVERYS; i++)
    {
        if(DeliveryInfo[i][deliveryValid])
        {
            if(i>endid)break;
            format(str,sizeof(str),"%s%d %s(%s(%d) - %s(%d)) %d %d$\n",
            str,
            i,
        	DeliveryInfo[i][deliveryProd],
			PropInfo[DeliveryInfo[i][deliveryStartProp]][propName],
		    DeliveryInfo[i][deliveryStartProp],
		    PropInfo[DeliveryInfo[i][deliveryEndProp]][propName],
		    DeliveryInfo[i][deliveryEndProp],
		    DeliveryInfo[i][deliveryFragility],
		    DeliveryInfo[i][deliveryPrice]);
		    bit=i;
        }
    }
    format(str,sizeof(str),"%s%d next",str,bit);
	new strm[255];
	format(strm,sizeof(strm),"������ �������� [%d - %d]",startid,endid);
    ShowPlayerDialog(playerid,DIALOG_DELIVERYLIST,DIALOG_STYLE_LIST,strm,str,"�����","�����");
	return 1;
}
forward OnTrailerAttach(playerid,trailerid,vehicleid);
forward OnTrailerDetach(playerid,trailerid,vehicleid);
new TrailerBit[MAX_PLAYERS];

stock TrailerUpdate(playerid,vehicleid)
{
	PlayerTrailer[playerid]=GetVehicleTrailer(vehicleid);
    if(PlayerTrailer[playerid]>0)
    {
		if(TrailerBit[playerid]==0)
		{
	        TrailerBit[playerid]=1;
	        OnTrailerAttach(playerid,PlayerTrailer[playerid],PlayerInfo[playerid][pCarID]);
        }
    }
    else
    {
		if(TrailerBit[playerid]==1)
	    {
	        OnTrailerDetach(playerid,PlayerTrailer[playerid],vehicleid);
	        PlayerTrailer[playerid]=0;
            TrailerBit[playerid]=0;
	    }
	}
	return 1;
}
public OnTrailerAttach(playerid,trailerid,vehicleid)
{
	if(PlayerInfo[playerid][pCarID]==vehicleid)
	{
	    for(new i=0; i<MAX_TRAILERS; i++)
		{
		    if(TrailerInfo[i][tiValid])
			{
			    if(TrailerInfo[i][tiCarID]==trailerid)
			    {
					new str[255];
					new model=GetVehicleModel(trailerid);
					if(PlayerInfo[playerid][pDeliveryID]==-1)
					{
						PlayerInfo[playerid][pTrailerID]=trailerid;
						switch(model)
						{
						    case 435, 591://�������
						    	format(str,sizeof(str),"�� ��������� ������� ������(%d), ����������� /delivery ��� ��������� ������",trailerid);
							case 450://�����
							    format(str,sizeof(str),"�� ��������� ������ ��������� ����(%d), ����������� /delivery ��� ��������� ������",trailerid);
							case 584://��������
							    format(str,sizeof(str),"�� ��������� ������ ��������(%d), ����������� /delivery ��� ��������� ������",trailerid);
						}
					}
					else
					{
					    if(TrailerInfo[i][tiCarID]!=PlayerInfo[playerid][pTrailerID])
						{
						   	switch(model)
							{
							    case 435, 591://�������
							    	format(str,sizeof(str),"�� ��������� ������� ������(%d), ��������, ��� �� ��� ������, ������� ����� � ��� �� ���������!",trailerid);
								case 450://�����
								    format(str,sizeof(str),"�� ��������� ������ ��������� ����(%d), ��������, ��� �� ��� ������, ������� ����� � ��� �� ���������!",trailerid);
								case 584://��������
								    format(str,sizeof(str),"�� ��������� ������ ��������(%d), ��������, ��� �� ��� ������, ������� ����� � ��� �� ���������!",trailerid);
							}
						}
						else
						{
						    PlayerInfo[playerid][pTrailerID]=trailerid;
						    DeathDeliveryTimer[playerid]=0;
						    switch(model)
							{
							    case 435, 591://�������
							    	format(str,sizeof(str),"�� ��������� ������� ������(%d), �� �������� ��� ������� ��������",trailerid);
								case 450://�����
								    format(str,sizeof(str),"�� ��������� ������ ��������� ����(%d), �� �������� ��� ������� ��������",trailerid);
								case 584://��������
								    format(str,sizeof(str),"�� ��������� ������ ��������(%d), �� �������� ��� ������� ��������",trailerid);
							}
						}
					}
					SendClientMessage(playerid,COLOR_YELLOW,str);
			        return 1;
			    }
			}
		}
	}
	return 1;
}
public OnTrailerDetach(playerid,trailerid,vehicleid)
{
    if(PlayerInfo[playerid][pDeliveryID]!=-1)
	{
	    new str[128];
		/*if(PlayerInfo[playerid][pTrailerID]==trailerid)
		{*/
	    DeathDeliveryTimer[playerid]=60;
        format(str,sizeof(str),"�� �������� ��� �������, � ��� ���� %d ������ ��� �� ��� ���������!",DeathDeliveryTimer[playerid]);
		/*}
		else
		{
		    DeathDeliveryTimer[playerid]+=20;
		    format(str,sizeof(str),"�� �������� ����� �������, � ��� �������� %d ������ ��� �� ��������� ����",DeathDeliveryTimer[playerid]);
		}*/
		SendClientMessage(playerid,COLOR_YELLOW,str);
	}
	else
	{
        PlayerInfo[playerid][pTrailerID]=0;
	    SendClientMessage(playerid,COLOR_YELLOW,"�� �������� �������");
	}
	return 1;
}

stock PlayerStartDelivery(playerid,bits)
{
	new trailer=3;
	new model=GetVehicleModel(PlayerInfo[playerid][pTrailerID]);
	switch(model)
	{
		case 435, 591://�������
	    	trailer=0;
		case 450://�����
		    trailer=1;
		case 584://��������
		    trailer=2;
	}
	/*if(trailer==-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "���������� ������!");*/
	new i;
	switch(bits)
	{
		case 0:
		{
			new delid[MAX_DELIVERYS],bit;
			new maxid;
			for(i=0; i<MAX_DELIVERYS; i++)
		    {
		        if(DeliveryInfo[i][deliveryValid])
		        {
		            if(DeliveryInfo[i][deliveryTrailer]==trailer)
		            {
		                delid[bit]=i;
						bit++;
						maxid=bit;
		            }
		        }
		    }
		    if(maxid==0)
		        return SendClientMessage(playerid, COLOR_GRAD2, "��� ���������� ��������!");
		    new b=random(maxid);
			i=delid[b];
		}
		case 1:
		{
		    new delid[MAX_DELIVERYS],bit;
		    new maxid;
		    for(i=0; i<MAX_DELIVERYS; i++)
		    {
		        if(DeliveryInfo[i][deliveryValid])
		        {
		            if(DeliveryInfo[i][deliveryTrailer]==trailer)
		            {
			            if(IsPlayerInRangeOfPoint(playerid,10.0,PropInfo[DeliveryInfo[i][deliveryStartProp]][propX],PropInfo[DeliveryInfo[i][deliveryStartProp]][propY],PropInfo[DeliveryInfo[i][deliveryStartProp]][propZ]))
						{
						    delid[bit]=i;
							bit++;
							maxid=bit;
						}
					}
		        }
		    }
		    if(bit == -1)
		        return SendClientMessage(playerid, COLOR_GRAD2, "��� ���������� ��������!");
			new b=random(maxid);
			i=delid[b];
		}
	}
    PlayerInfo[playerid][pDeliveryID]=i;
    new s=DeliveryInfo[i][deliveryStartProp];
    new e=DeliveryInfo[i][deliveryEndProp];
	new str[512];
    format(str,sizeof(str),"[�����] ������� %s, ��������� %d ���������, �������� �� %s � %s",
	DeliveryInfo[i][deliveryProd],
	DeliveryInfo[i][deliveryFragility],
	PropInfo[s][propName],
	PropInfo[e][propName]);
    SendClientMessage(playerid,COLOR_YELLOW,str);
    CP[playerid]=1;
    PlayerInfo[playerid][pDeliveryHealth]=0;
    SetPlayerCheckpoint(playerid, PropInfo[s][propX],PropInfo[s][propY],PropInfo[s][propZ], 10.0);
    format(str,sizeof(str),"~b~trucker.valakas.ru ~r~/help             ~y~Produkt: ~r~%s ~y~iz ~r~%s",
    Translate(DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd]),
    Translate(PropInfo[s][propName]));
    SetHelpDraw(playerid,str);
	return 1;
}
COMMAND:delivery(playerid,params[])
{
    if(PlayerInfo[playerid][pDeliveryID]!=-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "� ��� ��� ���� �����!");
	if(!IsPlayerInAnyVehicle(playerid))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ������!");
	if(GetPlayerVehicleID(playerid) != PlayerInfo[playerid][pCarID])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ����� ���������!");
    if(PlayerInfo[playerid][pTrailerID]==0 && IsATruck(PlayerInfo[playerid][pCarID]))
        return SendClientMessage(playerid, COLOR_GRAD2, "� ��� ��� ��������!");
    PlayerStartDelivery(playerid,0);
	return 1;
}
COMMAND:closestdelivery(playerid,params[])
{
	if(PlayerInfo[playerid][pPoints]<650)
	     return SendClientMessage(playerid, COLOR_GRAD2, "��� ���� ��� �� ����� ��������� �������� ����� ����� ����� 650 �����!");
    if(PlayerInfo[playerid][pDeliveryID]!=-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "� ��� ��� ���� �����!");
	if(!IsPlayerInAnyVehicle(playerid))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ������!");
	if(GetPlayerVehicleID(playerid) != PlayerInfo[playerid][pCarID])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ����� ���������!");
    if(PlayerInfo[playerid][pTrailerID]==0 && IsATruck(PlayerInfo[playerid][pCarID]))
        return SendClientMessage(playerid, COLOR_GRAD2, "� ��� ��� ��������!");
    PlayerStartDelivery(playerid,1);
    return 1;
}
stock PlayerDeathDelivery(playerid)//��������� ����������
{
    new str[255],Float:money,price;
    money=
    (
	CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1
	+
	DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1)*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryFragility]/100
	+
	(CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*
	DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1+
	DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1);
	//money=money/100*(100-PlayerInfo[playerid][pDeliveryHealth]); ��� �� �����, ��� ��� ����� ������� �� ���� ����
    format(str,sizeof(str),"%f",money);
	price=strval(str);
	oGivePlayerMoney(playerid,-price);
	format(str,sizeof(str),"�� ��������� ��������, � ��� �������� ��������� %d $",price);
    SendClientMessage(playerid,COLOR_YELLOW,str);
	PlayerInfo[playerid][pDeliveryID]=-1;
	DisablePlayerCheckpoint(playerid);
    CP[playerid]=0;
    SetHelpDraw(playerid,"~b~trucker.valakas.ru ~r~/help");
	return 1;
}
public OnVehicleDeath(vehicleid)
{
    foreach(Player,i)
    {
        if(PlayerInfo[i][pCarID]==vehicleid)
        {
            if(PlayerInfo[i][pDeliveryID]!=-1)
                PlayerDeathDelivery(i);
            return 1;
        }
    }
    return 1;
}
stock PlayerEndDelivery(playerid)
{
	new str[255],Float:money,price;
	if(PlayerInfo[playerid][pDeliveryHealth]>50.0)
	{
//        ����=(�����*����������*���������/100)%������� �����
        money=
	    (
		CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1
		+
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1)*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryFragility]/100
		+
		(CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1+
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1);
		money=money/100*PlayerInfo[playerid][pDeliveryHealth];
	    format(str,sizeof(str),"%f",money);
		price=strval(str);
		new convoy=CheckCountConvoy(playerid);
		new convoybonus;
		if(convoy)
		{
			convoybonus=price/100*convoy*5;
			oGivePlayerMoney(playerid,convoybonus);
		}
		oGivePlayerMoney(playerid,price);
		if(oGetPlayerMoney(playerid)>0)
	    {
		    if(IsATruck(GetPlayerVehicleID(playerid)))
		    {

		        format(str,sizeof(str),"����������� � ������� ���������, ��� ������� %d $, ��� �� ��������� ��� ���� ��������",price);
	    		PlayerInfo[playerid][pPoints]+=2;
			}
			else
			{
			    PlayerInfo[playerid][pPoints]++;
				format(str,sizeof(str),"����������� � ������� ���������, ��� ������� %d $, ��� �� ��������� ���� ���� ��������",price);
			}
		}
		else
		    format(str,sizeof(str),"����������� � ������� ���������, ��� ������� %d $",price);
		if(convoy)
			format(str,sizeof(str),"%s, ����� ������ %d $",str,convoybonus);
		SendClientMessage(playerid,COLOR_YELLOW,str);
    }
	else
	{
	    money=
	    (
		CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1
		+
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1)*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryFragility]/100
		+
		(CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]*
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryValue]*0.1+
		DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryPrice]*0.1);
		money=money/100*(100-PlayerInfo[playerid][pDeliveryHealth]);
	    format(str,sizeof(str),"%f",money);
		price=strval(str);
        oGivePlayerMoney(playerid,-price);
		format(str,sizeof(str),"���, �������� ���� ���������, ���� ������ ���������, ��������� %d $",price);
	    SendClientMessage(playerid,COLOR_YELLOW,str);
	}
    PlayerInfo[playerid][pDeliveryID]=-1;
	return 1;
}

public OnPlayerEnterCheckpoint(playerid)
{
	switch(CP[playerid])
	{
	    case 1://�������� � �������� ������ ���������� ������� ��������.
	    {
			if(!PlayerDeliveryState[playerid])
				PlayerDeliveryOnStart(playerid);
	    }
	    case 2://�������� � �������� ���� ���������� ��������� ��������.
	    {
	        if(!PlayerDeliveryState[playerid])
	        	PlayerDeliveryOnEnd(playerid);
	    }
	    case 3://����� ����������� ������.
	    {
	        DisablePlayerCheckpoint(playerid);
	    }
	}
	return 1;
}
stock PlayerDeliveryOnStart(playerid)
{
	if(GetPlayerVehicleID(playerid) != PlayerInfo[playerid][pCarID])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ����� ���������!");
	PlayerDeliveryState[playerid]=1;
	PlayerDeliveryStartTime[playerid]=(CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]/100);
	new str[255];
	format(str,sizeof(str),"���� �������� %s",DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd]);
	SendClientMessage(playerid,COLOR_YELLOW,str);
	return 1;
}
new Float:VehicleHealth[MAX_VEHICLES];
stock GetVehicleDamage(playerid,vehicleid)
{
	new Float:hp,Float:raz;
	GetVehicleHealth(vehicleid,hp);
	if(hp < VehicleHealth[vehicleid])
	{
	    PlayerInfo[playerid][pCarFullHealth]--;
	    if(PlayerInfo[playerid][pDeliveryID]!=-1)
	    {
	        if(PlayerInfo[playerid][pDeliveryHealth]>0.0)
	        {
				raz=VehicleHealth[vehicleid]-hp;
				PlayerInfo[playerid][pDeliveryHealth]-=raz*DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryFragility]/50;
				if(PlayerInfo[playerid][pDeliveryHealth] < 0.0)
				    PlayerInfo[playerid][pDeliveryHealth]=0.0;
			}
		}
	}
    VehicleHealth[vehicleid]=hp;
	return 1;
}

stock PlayerDeliveryStartTimerEnd(playerid)
{
    new e=DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryEndProp];
    new str[512];
	format(str,sizeof(str),"�������� %s ���������, ������������� � %s",DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd],PropInfo[e][propName]);
	SendClientMessage(playerid,COLOR_YELLOW,str);
    PlayerDeliveryState[playerid]=0;
    PlayerInfo[playerid][pDeliveryHealth]=100.0;
    CP[playerid]=2;
    SetPlayerCheckpoint(playerid, PropInfo[e][propX],PropInfo[e][propY],PropInfo[e][propZ], 10.0);
    format(str,sizeof(str),"~b~trucker.valakas.ru ~r~/help             ~y~Product: ~r~%s ~y~v ~r~%s",
    Translate(DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd]),
    Translate(PropInfo[e][propName]));
    SetHelpDraw(playerid,str);
	return 1;
}
stock PlayerDeliveryOnEnd(playerid)
{
    if(GetPlayerVehicleID(playerid) != PlayerInfo[playerid][pCarID])
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ����� ���������!");
	PlayerDeliveryState[playerid]=1;
	PlayerDeliveryEndTime[playerid]=(CarInfo[GetBuyIDFromModel(PlayerInfo[playerid][pCarModel])][carCapacity]/100);
	new str[255];
	format(str,sizeof(str),"���� ��������� %s",DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd]);
	SendClientMessage(playerid,COLOR_YELLOW,str);
	return 1;
}
stock PlayerDeliveryEndTimerEnd(playerid)
{
    new str[255];
	format(str,sizeof(str),"��������� %s ���������",DeliveryInfo[PlayerInfo[playerid][pDeliveryID]][deliveryProd]);
	SendClientMessage(playerid,COLOR_YELLOW,str);
    DisablePlayerCheckpoint(playerid);
    PlayerDeliveryState[playerid]=0;
    CP[playerid]=0;
    SetHelpDraw(playerid,"~b~trucker.valakas.ru ~r~/help");
    PlayerEndDelivery(playerid);
    return 1;
}