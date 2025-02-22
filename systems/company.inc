
enum companyinfo
{
	companyValid,
	companyOwner[MAX_PLAYER_NAME],
	companyName[64],
	companyOfficeID,
	companyBank,
};
new CompanyInfo[MAX_COMPANY][companyinfo];
stock LoadCompany(i)
{
    new query[255];
	format(query,sizeof(query),"SELECT companyOwner, companyName, companyOfficeID, companyBank FROM company WHERE id=%d",i);
	mysql_tquery(MySQL, query, "SQL_LoadCompany", "d", i);
	return 1;
}
forward SQL_LoadCompany(i);
public SQL_LoadCompany(i)
{
	new rows;
	cache_get_row_count(rows);
	if(rows>0)
	{
		//sscanf(Field,"p<|>s[24]s[64]i",
		cache_get_value_name(0,"companyOwner",CompanyInfo[i][companyOwner],24);
		cache_get_value_name(0,"companyName",CompanyInfo[i][companyName],64);
		cache_get_value_name_int(0,"companyOfficeID",CompanyInfo[i][companyOfficeID]);
		cache_get_value_name_int(0,"companyBank",CompanyInfo[i][companyBank]);
		CompanyInfo[i][companyValid]=1;
	}
	else
	    CompanyInfo[i][companyValid]=0;
	return 1;
}
stock CompanyInit()
{
    for(new i=0; i<MAX_COMPANY; i++)
        LoadCompany(i);
    return 1;
}
stock SaveCompany(i)
{
	if(CompanyInfo[i][companyValid])
	{
	    new query[255];
	    format(query,sizeof(query),"UPDATE company SET companyOwner='%s', companyName='%s', companyOfficeID=%d, companyBank=%d WHERE id=%d",
		CompanyInfo[i][companyOwner],
		CompanyInfo[i][companyName],
		CompanyInfo[i][companyOfficeID],
		CompanyInfo[i][companyBank],
		i);
		mysql_query(MySQL, query, false);
	}
	return 1;
}

stock CreateCompany(playerid,name[])
{
    for(new i=0; i<MAX_COMPANY; i++)
    {
        if(!CompanyInfo[i][companyValid])
		{
		    new query[255];
	    	format(query,sizeof(query),"INSERT INTO company SET companyOwner='%s', companyName='%s', companyOfficeID=%d, id=%d",
			oGetPlayerName(playerid),
			name,
			-1,
			i);
			mysql_query(MySQL, query, false);
			LoadCompany(i);
			return i;
		}
    }
	return 1;
}
COMMAND:opencompany(playerid,params[])
{
	if(PlayerInfo[playerid][pPoints] < 1000)
	    return SendClientMessage(playerid, COLOR_GRAD2, "� ��� ������������ ����� �������� ��� �� ������� �������� (��������� 1000 � ����).");
	new name[64];
	if(sscanf(params,"s[64]",name))
	    return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /opencompany [�������� ��������] (�������� �������� ����� 1000000 $)");
	if(oGetPlayerMoney(playerid) < 1000000)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�������� �������� ����� 1000000 $");
	new i=CreateCompany(playerid,name);
	oGivePlayerMoney(playerid,-1000000);
	PlayerInfo[playerid][pCompany]=i;
	new str[255];
	format(str,sizeof(str),"�� ������� �������� %s (%d)",
	CompanyInfo[i][companyName],
	i);
	SendClientMessage(playerid, COLOR_GRAD2, str);
	return 1;
}
COMMAND:closecompany(playerid,params[])
{
    if(PlayerInfo[playerid][pCompany]==-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� �������� �� � ����� ��������.");
	if(strcmp(CompanyInfo[PlayerInfo[playerid][pCompany]][companyOwner],oGetPlayerName(playerid), true ) == 0 )
	{
	    new str[255];
	    format(str,sizeof(str),"UPDATE players SET pCompany='-1', pCompanyTime=0 WHERE online=0 AND pCompany=%d",
	    PlayerInfo[playerid][pCompany]);
	    mysql_query(MySQL, str, false);
	    foreach(Player,i)
		{
		    if(PlayerInfo[i][pCompany]==PlayerInfo[playerid][pCompany])
			{
			    PlayerInfo[playerid][pCompany]=0;
			    PlayerInfo[playerid][pCompanyTime]=0;
			    SendClientMessage(i,COLOR_WHITE,"���� �������� ���������, ��� ������������� �������");
			}
		}
		return 1;
	}
	else
	    SendClientMessage(playerid, COLOR_GRAD2, "�� �� ��������� ������ ������� ��������.");
	return 1;
}
COMMAND:invite(playerid,params[])
{
	if(PlayerInfo[playerid][pCompany]==-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� �������� �� � ����� ��������.");
	if(strcmp(CompanyInfo[PlayerInfo[playerid][pCompany]][companyOwner],oGetPlayerName(playerid), true ) == 0 )
	{
		if(CheckCompanyMembersNum(PlayerInfo[playerid][pCompany]) == CheckCompanyMaxMembers(playerid))
            return SendClientMessage(playerid, COLOR_GRAD2, "� ����� �������� �� ������ ������ ������������ ���������� �����������.");
		new giveplayerid;
		if(sscanf(params,"u",giveplayerid))
		    return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /invite [ID ������/����� �����]");
		if(PlayerInfo[giveplayerid][pCompany]!=-1)
		    return SendClientMessage(playerid, COLOR_GRAD2, "����� ��� ����� �����-�� ���������.");
        InviteOffer[playerid]=giveplayerid;
        InviteOffer[giveplayerid]=playerid;
        new str[512];
        format(str,sizeof(str),"{FFFFFF}%s ��������� ��� �������� � �������� %s.\n��������� ����� ������������ ���������� %d �����.\n������������ ������ � ��������:\n�� ������ �������� �� ��������� 10 ���������� ����� � ��������.\n�� ������ ���� ��������, �������� �������� �������������� 5 ��������� �� ����� �������.",
		GetPlayerNameEx(playerid),
		CompanyInfo[PlayerInfo[playerid][pCompany]][companyName],
		PlayerInfo[playerid][pPoints]/60);
		ShowPlayerDialog(giveplayerid,DIALOG_INVITE,DIALOG_STYLE_MSGBOX,"����������� ������",str,"{33AA33}[�������]", "{AA3333}[���������]");
		format(str,sizeof(str),"�� ���������� ������ %s",GetPlayerNameEx(giveplayerid));
		SendClientMessage(playerid,COLOR_WHITE,str);
		//�����
	}
	else
	    SendClientMessage(playerid, COLOR_GRAD2, "�� �� ��������� ������ ������� ��������.");
	return 1;
}

stock CheckCompanyMembersNum(i)
{
	new str[255];
	format(str,sizeof(str),"SELECT id FROM players WHERE pCompany=%d",i);
    new Cache:result=mysql_query(MySQL,str);
    new rows;
	cache_get_row_count(rows);
	cache_delete(result);
	return rows;
}
stock CheckCompanyMaxMembers(playerid)
{
	new num=PlayerInfo[playerid][pPoints]/500;
	return num;
}

stock AcceptInvite(playerid)
{
	new giveplayerid=InviteOffer[playerid];
    PlayerInfo[playerid][pCompanyTime]=PlayerInfo[giveplayerid][pPoints];
    PlayerInfo[playerid][pCompany]=PlayerInfo[giveplayerid][pCompany];
	new str[255];
	format(str,sizeof(str),"%s ������ ���� ����������� ������, ����� ��������� %d �����",
		GetPlayerNameEx(playerid),
		PlayerInfo[playerid][pCompanyTime]/60
	);
    ShowPlayerDialog(giveplayerid,0,DIALOG_STYLE_MSGBOX,"����������� ������",str,"��","");
    format(str,sizeof(str),"�� ������� �����������, ������ �� ��������� �� �������� %s\n����� ��������� %d �����",
	    CompanyInfo[PlayerInfo[playerid][pCompany]][companyName],
		PlayerInfo[playerid][pCompanyTime]/60
	);
	ShowPlayerDialog(giveplayerid,0,DIALOG_STYLE_MSGBOX,"����������� ������",str,"��","");
	InviteOffer[playerid]=INVALID_PLAYER_ID;
    InviteOffer[giveplayerid]=INVALID_PLAYER_ID;
	return 1;
}
stock CancelInvite(playerid)
{
    new giveplayerid=InviteOffer[playerid];
	new str[255];
    format(str,sizeof(str),"%s �������� ���� ����������� ������",
		GetPlayerNameEx(playerid)
	);
    ShowPlayerDialog(giveplayerid,0,DIALOG_STYLE_MSGBOX,"����������� ������",str,"��","");
    ShowPlayerDialog(playerid,0,DIALOG_STYLE_MSGBOX,"����������� ������","�� ��������� ����������� ������","��","");
    InviteOffer[playerid]=INVALID_PLAYER_ID;
    InviteOffer[giveplayerid]=INVALID_PLAYER_ID;
	return 1;
}

stock UpdateCompanys()
{
	foreach(Player,i)
	{
	    if(PlayerInfo[i][pCompanyTime]>0)
	        PlayerInfo[i][pCompanyTime]--;
	}
	mysql_query(MySQL,"UPDATE players SET 'pCompanyTime'='pCompanyTime' - 1 WHERE pCompanyTime>0 AND online=0",false);
	mysql_query(MySQL,"UPDATE players SET pCompany='-1' WHERE pCompanyTime=0 AND online=0 AND pCompany!='-1'",false);
	return 1;
}
COMMAND:companywithdraw(playerid,params[])
{
    if(!IsPlayerInAtm(playerid))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ���������");
    if(PlayerInfo[playerid][pCompany]==-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� �������� �� � ����� ��������.");
	if(strcmp(CompanyInfo[PlayerInfo[playerid][pCompany]][companyOwner],oGetPlayerName(playerid), true ) == 0 )
	{
	    new money, str[255];
		if(sscanf(params,"i",money))
		{
		    format(str,sizeof(str)," �����������: /companywithdraw [�����] (����� �� ����� %d $)",money);
		    return SendClientMessage(playerid, COLOR_GRAD2, str);
		}
		if(money < 1 || money > CompanyInfo[PlayerInfo[playerid][pCompany]][companyBank])
		    return SendClientMessage(playerid, COLOR_GRAD2, "��� ������� �����.");
		CompanyInfo[PlayerInfo[playerid][pCompany]][companyBank]-=money;
		oGivePlayerMoney(playerid,money);
		format(str,sizeof(str),"�� ����� �� ����� �������� %d $",money);
		SendClientMessage(playerid,COLOR_WHITE,str);
		SaveCompany(PlayerInfo[playerid][pCompany]);
		return 1;
	}
	else
	    SendClientMessage(playerid, COLOR_GRAD2, "�� �� ��������� ������ ������� ��������.");
	return 1;
}
COMMAND:companybank(playerid,params[])
{
    if(!IsPlayerInAtm(playerid))
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� � ���������");
    if(PlayerInfo[playerid][pCompany]==-1)
	    return SendClientMessage(playerid, COLOR_GRAD2, "�� �� �������� �� � ����� ��������.");
	if(strcmp(CompanyInfo[PlayerInfo[playerid][pCompany]][companyOwner],oGetPlayerName(playerid), true ) == 0 )
	{
	    new money, str[255];
		if(sscanf(params,"i",money))
		    return SendClientMessage(playerid, COLOR_GRAD2, " �����������: /companybank [�����]");
		if(money < 1 || money > oGetPlayerMoney(playerid))
		    return SendClientMessage(playerid, COLOR_GRAD2, "��� ������� �����.");
		CompanyInfo[PlayerInfo[playerid][pCompany]][companyBank]+=money;
		oGivePlayerMoney(playerid,-money);
		format(str,sizeof(str),"�� ����� �� ����� �������� %d $",money);
		SendClientMessage(playerid,COLOR_WHITE,str);
		SaveCompany(PlayerInfo[playerid][pCompany]);
		return 1;
	}
	else
	    SendClientMessage(playerid, COLOR_GRAD2, "�� �� ��������� ������ ������� ��������.");
	return 1;
}