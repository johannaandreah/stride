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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Helpers for age.
 */

#pragma once

namespace stride {

/// Maximum age for Persons.
inline constexpr unsigned int MaximumAge() { return 109U;}

/// Maximum age for Person's.
inline constexpr unsigned int MinAdultAge() { return 18U; }

/// Effective age (topping of at maximum).
inline unsigned int EffectiveAge(unsigned int age) { return (age <= MaximumAge()) ? age : MaximumAge(); }

/// Maximum role (age/gender) category for ERGM household contact matrix.
inline constexpr unsigned int RoleCategory() { return 7U;}

/// Household role from age and sex; returns 0 when sex is not recognized for ages 25-60.
inline constexpr unsigned int HouseholdRole(unsigned int age, unsigned int sex)
{
        if (age <= 5U) {
                return 1U;
        }
        if (age <= 12U) {
                return 2U;
        }
        if (age <= 18U) {
                return 3U;
        }
        if (age <= 24U) {
                return 4U;
        }
        if (age <= 60U) {
                return sex == 2U ? 5U : sex == 1U ? 6U : 0U;
        }
        return 7U;
}

} // namespace stride
