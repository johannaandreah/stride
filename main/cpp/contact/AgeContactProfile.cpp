/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2024
 */

/**
 * @file
 * Contact profile.
 */

#include "AgeContactProfile.h"

#include "util/Ptree.h"

#include <stdexcept>
#include <string>

namespace stride {

using namespace std;
using namespace stride::ContactType;
using namespace stride::util;

namespace {

string GetContactTypeKey(Id poolType)
{
        string typeKey = "";
        // TODO Eliminate this switch by fixing the data file
        if (poolType == Id::School) {
                typeKey = "school";
        } else if (poolType == Id::Household) {
                typeKey = "household";
        } else if (poolType == Id::Workplace) {
                typeKey = "workplace";
        } else if (poolType == Id::CommunityWeekend) {
                typeKey = "community_weekend";
        } else if (poolType == Id::CommunityWeekday) {
                typeKey = "community_weekday";
        } else if (poolType == Id::HouseholdCluster) {
             	typeKey = "household";
        } else if (poolType == Id::Collectivity) {
         		typeKey = "collectivity";
        }
        return typeKey;
}

string GetContactTypeKey(ERGMContactType::Id poolType)
{
        string typeKey = "";
        // TODO Eliminate this switch by fixing the data file
        if (poolType == ERGMContactType::Id::Weekday_2) {
                typeKey = "ergm_weekday_2";
        } else if (poolType == ERGMContactType::Id::Weekday_3) {
                typeKey = "ergm_weekday_3";
        } else if (poolType == ERGMContactType::Id::Weekday_4) {
                typeKey = "ergm_weekday_4";
        } else if (poolType == ERGMContactType::Id::Weekday_5) {
                typeKey = "ergm_weekday_5";
        } else if (poolType == ERGMContactType::Id::Weekday_6) {
                typeKey = "ergm_weekday_6";
        } else if (poolType == ERGMContactType::Id::Weekend_2) {
                typeKey = "ergm_weekend_2";
        } else if (poolType == ERGMContactType::Id::Weekend_3) {
                typeKey = "ergm_weekend_3";
        } else if (poolType == ERGMContactType::Id::Weekend_4) {
                typeKey = "ergm_weekend_4";
        } else if (poolType == ERGMContactType::Id::Weekend_5) {
                typeKey = "ergm_weekend_5";
        } else if (poolType == ERGMContactType::Id::Weekend_6) {
                typeKey = "ergm_weekend_6";
        }
        return typeKey;
}

double GetContactRateAdjustment(const string& typeKey, const ptree& contactPt)
{
        double contactRateAdjustment = 1.0;
        const string key_adj{string("matrices.adjustment_factor.").append(typeKey).append(".value")};
        if (const auto val = contactPt.get_optional<double>(key_adj)) {
                contactRateAdjustment = *val;
                cerr << "UPDATE: the contact adjustment factor for " << key_adj << ": " << contactRateAdjustment << endl;
        }
        return contactRateAdjustment;
}

} // namespace

AgeContactProfile::AgeContactProfile(Id poolType, const ptree& contactPt) : std::array<double, MaximumAge() + 1>()
{
        const string typeKey = GetContactTypeKey(poolType);
        const double contactRateAdjustment = GetContactRateAdjustment(typeKey, contactPt);

        // construct XML key for contact rates
        const string key{string("matrices.").append(typeKey)};

        // if the XML key is present, parse ptree and store data
        if(contactPt.get_optional<std::string>(key).has_value()){
        	unsigned int i = 0U;
			for (const auto& participant : contactPt.get_child(key)) {
					double totalContacts = 0;
					for (const auto& contact : participant.second.get_child("contacts")) {
							totalContacts += contact.second.get<double>("rate");
					}
					(*this)[i++] = totalContacts * contactRateAdjustment;
			}

        } else {
                cerr << "WARNING: the contact data does not contain '" << key << "', assuming contact rate == 0" << endl;
        }
}

AgeContactProfileERGM::AgeContactProfileERGM(ERGMContactType::Id poolType, const ptree& contactPt)
    : std::array<std::array<double, RoleCategory() + 1>, RoleCategory() + 1>()
{
        const string typeKey = GetContactTypeKey(poolType);
        const double contactRateAdjustment = GetContactRateAdjustment(typeKey, contactPt);
        const string key{string("matrices.").append(typeKey)};

        if (contactPt.get_optional<std::string>(key).has_value()) {
                size_t participantIndex = 1U;
                for (const auto& participant : contactPt.get_child(key)) {
                        if (participantIndex >= size()) {
                                throw std::runtime_error("AgeContactProfileERGM> too many participant rows in '" +
                                                         key + "'");
                        }
                        size_t contactIndex = 1U;
                        for (const auto& contact : participant.second.get_child("contacts")) {
                                if (contactIndex >= (*this)[participantIndex].size()) {
                                        throw std::runtime_error(
                                            "AgeContactProfileERGM> too many contacts in row " +
                                            std::to_string(participantIndex) + " of '" + key + "'");
                                }
                                (*this)[participantIndex][contactIndex++] =
                                    contact.second.get<double>("rate") * contactRateAdjustment;
                        }
                        ++participantIndex;
                }
        } else {
                cerr << "WARNING: the contact data does not contain '" << key << "', assuming contact rate == 0" << endl;
        }
}

} // namespace stride
