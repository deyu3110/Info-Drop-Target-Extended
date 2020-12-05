//search:
	if (id == 0)

//add before:
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
	if (iRarePct)
		item->SetRarity(iRarePct);
#endif

//search:
bool ITEM_MANAGER::CreateDropItemVector(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM> & vec_item)

//replace:
#ifdef __SEND_TARGET_INFO__
bool ITEM_MANAGER::CreateDropItemVector(LPCHARACTER pkChr, LPCHARACTER pkKiller, std::vector<LPITEM> & vec_item)
{
	if (pkChr->IsPolymorphed() || pkChr->IsPC())
		return false;

	int iLevel = pkKiller->GetLevel();
	BYTE bRank = pkChr->GetMobRank();
	LPITEM item = NULL;
	std::vector<CItemDropInfo>::iterator it = g_vec_pkCommonDropItem[bRank].begin();

	while (it != g_vec_pkCommonDropItem[bRank].end())
	{
		const CItemDropInfo & c_rInfo = *(it++);

		if (iLevel < c_rInfo.m_iLevelStart || iLevel > c_rInfo.m_iLevelEnd)
			continue;

		TItemTable * table = GetTable(c_rInfo.m_dwVnum);

		if (!table)
			continue;

		item = NULL;

		int dwPct = -1;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
		dwPct = c_rInfo.m_iPercent;
#endif

		if (table->bType == ITEM_POLYMORPH)
		{
			if (c_rInfo.m_dwVnum == pkChr->GetPolymorphItemVnum())
			{
				item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true, dwPct);

				if (item)
					item->SetSocket(0, pkChr->GetRaceNum());
			}
		}
		else
			item = CreateItem(c_rInfo.m_dwVnum, 1, 0, true, dwPct);

		if (item) vec_item.push_back(item);
	}

	// Drop Item Group
	{
		itertype(m_map_pkDropItemGroup) it;
		it = m_map_pkDropItemGroup.find(pkChr->GetRaceNum());

		if (it != m_map_pkDropItemGroup.end())
		{
			__typeof(it->second->GetVector()) v = it->second->GetVector();

			for (DWORD i = 0; i < v.size(); ++i)
			{
				int dwPct = -1;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
				dwPct = v[i].dwPct;
#endif

#ifdef ENABLE_EXTRA_DROP
				item = CreateItem(number(v[i].dwVnumStart, v[i].dwVnumEnd), v[i].iCount, 0, true, dwPct);
#else
				item = CreateItem(v[i].dwVnum, v[i].iCount, 0, true, dwPct);
#endif

				if (item)
				{
					if (item->GetType() == ITEM_POLYMORPH)
					{
						if (item->GetVnum() == pkChr->GetPolymorphItemVnum())
						{
							item->SetSocket(0, pkChr->GetRaceNum());
						}
					}

					vec_item.push_back(item);
				}
			}
		}
	}

	// MobDropItem Group
	{
		itertype(m_map_pkMobItemGroup) it;
		it = m_map_pkMobItemGroup.find(pkChr->GetRaceNum());

		if ( it != m_map_pkMobItemGroup.end() )
		{
#ifdef ENABLE_EXTRA_DROP
			std::vector<CMobItemGroup*>& vec_pGroups = it->second;

			for (int i = 0; i < vec_pGroups.size(); ++i)
			{
				CMobItemGroup* pGroup = vec_pGroups[i];
#else
				CMobItemGroup* pGroup = it->second;
#endif

				if (pGroup && !pGroup->IsEmpty())
				{
					const CMobItemGroup::SMobItemGroupInfo& info = pGroup->GetOne();
					
					int iRarePct = -1;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
					iRarePct = info.iRarePct;
#endif

#ifdef ENABLE_EXTRA_DROP
					item = CreateItem(number(info.dwItemVnumStart, info.dwItemVnumEnd), info.iCount, 0, true, iRarePct);
#else
					item = CreateItem(info.dwItemVnum, info.iCount, 0, true, iRarePct);
#endif
					if (item) vec_item.push_back(item);
				}
#ifdef ENABLE_EXTRA_DROP
			}
#endif
		}
	}

	// Level Item Group
	{
		itertype(m_map_pkLevelItemGroup) it;
		it = m_map_pkLevelItemGroup.find(pkChr->GetRaceNum());

		if ( it != m_map_pkLevelItemGroup.end() )
		{
			if ( it->second->GetLevelLimit() <= (DWORD)iLevel )
			{
				__typeof(it->second->GetVector()) v = it->second->GetVector();

				for ( DWORD i=0; i < v.size(); i++ )
				{
					int dwPct = -1;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
					dwPct = v[i].dwPct;
#endif

#ifdef ENABLE_EXTRA_DROP
					DWORD dwVnum = number(v[i].dwVNumStart, v[i].dwVNumEnd);
#else
					DWORD dwVnum = v[i].dwVNum;
#endif
					item = CreateItem(dwVnum, v[i].iCount, 0, true, dwPct);
					if ( item ) vec_item.push_back(item);
				}
			}
		}
	}

	// BuyerTheitGloves Item Group
	{
		if (pkKiller->GetPremiumRemainSeconds(PREMIUM_ITEM) > 0 || pkKiller->IsEquipUniqueGroup(UNIQUE_GROUP_DOUBLE_ITEM))
		{
			itertype(m_map_pkGloveItemGroup) it;
			it = m_map_pkGloveItemGroup.find(pkChr->GetRaceNum());

			if (it != m_map_pkGloveItemGroup.end())
			{
				__typeof(it->second->GetVector()) v = it->second->GetVector();

				for (DWORD i = 0; i < v.size(); ++i)
				{
					int dwPct = -1;
#ifdef ENABLE_SEND_TARGET_INFO_EXTENDED
					dwPct = v[i].dwPct;
#endif

#ifdef ENABLE_EXTRA_DROP
					DWORD dwVnum = number(v[i].dwVnumStart, v[i].dwVnumEnd);
#else
					DWORD dwVnum = v[i].dwVnum;
#endif
					item = CreateItem(dwVnum, v[i].iCount, 0, true, dwPct);
					if (item) vec_item.push_back(item);
				}
			}
		}
	}

	// ??
	if (pkChr->GetMobDropItemVnum())
	{
		itertype(m_map_dwEtcItemDropProb) it = m_map_dwEtcItemDropProb.find(pkChr->GetMobDropItemVnum());

		if (it != m_map_dwEtcItemDropProb.end())
		{		
			item = CreateItem(pkChr->GetMobDropItemVnum(), 1, 0, true);
			if (item) vec_item.push_back(item);
		}
	}

	if (pkChr->IsStone())
	{
		if (pkChr->GetDropMetinStoneVnum())
		{
			item = CreateItem(pkChr->GetDropMetinStoneVnum(), 1, 0, true);
			if (item) vec_item.push_back(item);
		}
	}

	return vec_item.size();
}
#endif


