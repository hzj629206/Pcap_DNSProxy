﻿// This code is part of Pcap_DNSProxy(Windows)
// Pcap_DNSProxy, A local DNS server base on WinPcap and LibPcap.
// Copyright (C) 2012-2015 Chengr28
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


#include "Base.h"

//Base defines
//Read Texts input and label types defines
#define READTEXT_PARAMETER             0
#define READTEXT_HOSTS                 1U
#define READTEXT_IPFILTER              2U
#define LABEL_STOP                     1U
#define LABEL_IPFILTER                 2U
#define LABEL_IPFILTER_BLACKLIST       3U
#define LABEL_IPFILTER_LOCAL_ROUTING   4U
#define LABEL_HOSTS                    5U
#define LABEL_HOSTS_LOCAL              6U
#define LABEL_HOSTS_WHITELIST          7U
#define LABEL_HOSTS_BANNED             8U
#define LABEL_HOSTS_ADDRESS            9U

//Length defines
#define READ_DATA_MINSIZE                     4U
#define READ_TEXT_MINSIZE                     2U
#define READ_PARAMETER_MINSIZE                8U
#define READ_HOSTS_MINSIZE                    3U
#define READ_IPFILTER_MINSIZE                 5U
#define READ_IPFILTER_BLACKLIST_MINSIZE       3U
#define READ_IPFILTER_LOCAL_ROUTING_MINSIZE   4U

//Global variables
extern CONFIGURATION_TABLE Parameter;
extern std::vector<std::wstring> ConfigFileList;
extern std::vector<FILE_DATA> IPFilterFileList, HostsFileList;
extern DNSCURVE_CONFIGURATON_TABLE DNSCurveParameter;
extern std::vector<HOSTS_TABLE> *HostsListUsing, *HostsListModificating;
extern std::vector<ADDRESS_RANGE_TABLE> *AddressRangeUsing, *AddressRangeModificating;
extern std::vector<RESULT_BLACKLIST_TABLE> *ResultBlacklistUsing, *ResultBlacklistModificating;
extern std::vector<ADDRESS_PREFIX_BLOCK> *LocalRoutingListUsing, *LocalRoutingListModificating;
extern std::vector<ADDRESS_HOSTS_TABLE> *AddressHostsListUsing, *AddressHostsListModificating;
extern std::deque<DNSCACHE_DATA> DNSCacheList;
extern std::mutex HostsListLock, AddressRangeLock, DNSCacheListLock, ResultBlacklistLock, LocalRoutingListLock, AddressHostsListLock;

//Functions
size_t __fastcall ReadParameterData(const PSTR Buffer, const size_t FileIndex, const size_t Line, bool &IsLabelComments);
bool __fastcall ReadText(const FILE *Input, const size_t InputType, const size_t FileIndex);
size_t __fastcall ReadMultiLineComments(const PSTR Buffer, std::string &Data, bool &IsLabelComments);
size_t __fastcall ReadIPFilterData(const PSTR Buffer, const size_t FileIndex, const size_t Line, size_t &LabelType, bool &IsLabelComments);
size_t __fastcall ReadBlacklistData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadLocalRoutingData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadMainIPFilterData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadHostsData(const PSTR Buffer, const size_t FileIndex, const size_t Line, size_t &LabelType, bool &IsLabelComments);
size_t __fastcall ReadWhitelistAndBannedData(std::string Data, const size_t FileIndex, const size_t Line, const size_t LabelType);
size_t __fastcall ReadLocalHostsData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadAddressHostsData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadMainHostsData(std::string Data, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadFileName(std::string Data, const size_t DataOffset, std::vector<std::wstring> *ListData, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadListenAddress(std::string Data, const size_t DataOffset, const uint16_t Protocol, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadSingleAddress(std::string Data, const size_t DataOffset, sockaddr_storage &SockAddr, const uint16_t Protocol, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadMultipleAddresses(std::string Data, const size_t DataOffset, sockaddr_storage &SockAddr, const uint16_t Protocol, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadHopLimitData(std::string Data, const size_t DataOffset, uint8_t &HopLimit, const uint16_t Protocol, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadDNSCurveProviderName(std::string Data, const size_t DataOffset, PSTR ProviderNameData, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadDNSCurveKey(std::string Data, const size_t DataOffset, PUINT8 KeyData, const size_t FileIndex, const size_t Line);
size_t __fastcall ReadMagicNumber(std::string Data, const size_t DataOffset, PSTR MagicNumber, const size_t FileIndex, const size_t Line);
