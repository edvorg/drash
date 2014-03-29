// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012-2014 Edward Knyshov, Yuriy Shatilin.

This file is part of the drash GPL Source Code (drash Source Code).

drash Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

drash Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with drash Source Code.  If not, see <http://www.gnu.org/licenses/>.

*/
// DRASH_LICENSE_END

#ifndef DRASH_APPEVENTSYSTEM_H
#define DRASH_APPEVENTSYSTEM_H

#include "appeventprocessor.h"
#include "appeventcombination.h"
#include <list>
#include <map>

namespace drash {

#define STATE_NORMAL 0x01
#define STATE_BEGIN 0x02
#define STATE_PROCESSING 0x04
#define STATE_END 0x08
#define STATE_CANCEL 0x10

    class CAppEvent;

    class CAppEventCombinationTree {
    public:
        friend class CAppEventSystem;

        CAppEventCombinationTree();

    protected:
    private:
        CAppEventProcessor processor;
        CAppEventCombination combination;
        std::list<CAppEventCombinationTree> childs;
        int state = STATE_NORMAL;
    };

    class CAppEventSystem {
    public:
        CAppEventSystem();

        static bool ValidateModeName(const std::string& _name);

        bool SetMode(const std::string& _name);
        void SetProcessor(const char* _combinations,
                          const CAppEventProcessor& _processor);

        void Process();

        void BeginEvent(const CAppEvent& _event);
        void EndEvent(const CAppEvent& _event);
        void CancelEvent(const CAppEvent& _event);

    protected:
    private:
        int PressEventImpl(const CAppEvent& _event);

        /// contains all current events
        /// BeginEvent invokation adds event to currentState
        /// EndEvent invokation removes event from currentState
        CAppEventCombination currentState;

        /// contains combinations being processed
        std::list<CAppEventCombinationTree*> currentCombinations;

        /// name of mode currently in use. mode is just name for one
        /// combinations tree.
        /// the reason for using of modes is processor conflicts when we need to
        /// bind
        /// more than one CAppEventProcessor instances to the same combination
        /// but execute only one instance at time
        std::string currentMode = "";

        /// combination trees for each mode
        std::map<std::string, CAppEventCombinationTree> trees;

        /// root of current mode tree
        CAppEventCombinationTree* currentModeRoot = nullptr;

        /// start point for searching of combinations to process
        CAppEventCombinationTree* currentNode = nullptr;
    };

} // namespace drash

#endif // DRASH_APPEVENTSYSTEM_H
