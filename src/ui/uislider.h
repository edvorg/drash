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

#ifndef DRASH_UISLIDER_H
#define DRASH_UISLIDER_H

#include "uiwidget.h"

namespace drash {

    namespace ui {

        class UISlider : public UIWidget {
        public:
            UISlider() = default;
            UISlider(const UISlider&) = delete;
            UISlider(UISlider&&) = delete;
            UISlider& operator=(const UISlider&) = delete;
            UISlider& operator=(UISlider&&) = delete;

            virtual void Connect(UISystem* _system) override;

            void
            SetValueHandler(const std::function<void(float _value)>& _handler);

            void SetValue(float _value);
            float GetValue() const;
            void SetPercent(float _percent);
            float GetPercent() const;
            void SetSliderWidth(unsigned int _width);
            unsigned int GetSliderWidth() const;
            void SetMin(float _min);
            float GetMin() const;
            void SetMax(float _max);
            float GetMax() const;

        protected:
        private:
            bool pressed = false;
            bool catched = false;
            float min = 0;
            float max = 100;
            float value = 0;
            unsigned int sliderWidth = 20;
            std::function<void(float _value)> valueHandler = [](float) {};
        };

    } // namepsace ui

} // namespace drash

#endif // DRASH_UISLIDER_H
