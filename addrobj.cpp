/*
    Smithproxy- transparent proxy with SSL inspection capabilities.
    Copyright (c) 2014, Ales Stibal <astib@mag0.net>, All rights reserved.

    Smithproxy is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Smithproxy is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Smithproxy.  If not, see <http://www.gnu.org/licenses/>.
    
*/    

#include <addrobj.hpp>
#include <dns.hpp>

int CidrAddress::contains(CIDR* other) {
    return cidr_contains(c_,other);
}


std::string FqdnAddress::to_string(int verbosity) {
    std::string ret = "FqdnAddress: " + fqdn_;
 
    if(verbosity > INF) {
        if(inspect_dns_cache.get(fqdn_) != nullptr) {
            ret += " (cached)";
        } else {
            ret += " (not cached)";
        }
    }
    
    return ret;
}


bool FqdnAddress::match(CIDR* c) {
    bool ret = false;
    
    DNS_Response* r = inspect_dns_cache.get(fqdn_);
    if(r != nullptr) {
        DEB_("FqdnAddress::match: found in cache: %s",fqdn_.c_str());
        
        std::vector<CidrAddress*> ips = r->get_a_anwsers();
        
        int i = 0;
        for(CidrAddress* ip: ips) {
            if(ip->match(c)) {
                DEB_("FqdnAddress::match: cached %s matches answer[%d] with %s",fqdn_.c_str(),i,ip->to_string().c_str());
                ret = true;
            } else {
                DEB_("FqdnAddress::match: cached %s DOESN'T match answer[%d] with %s",fqdn_.c_str(),i,ip->to_string().c_str());
            }
            ++i;
            // delete it straigt away.
            delete ip;
        }
        
    } else {
        DEB_("FqdnAddress::match: NOT found in cache: %s",fqdn_.c_str());
    }
    
    return ret;
}
