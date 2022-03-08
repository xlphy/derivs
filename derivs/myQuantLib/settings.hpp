//
//  settings.hpp
//  derivs
//
//  Created by Xin Li on 3/8/22.
//

#ifndef settings_hpp
#define settings_hpp

#include "singleton.hpp"
#include "observer.hpp"
#include "date.hpp"
#include <boost/optional.hpp>

namespace myQuantLib {

// global repository for run-time library settings
// Settings is a Singleton, using curiously recurring template pattern

class Settings: public Singleton<Settings> {
    friend class Singleton<Settings>;
private:
    Settings() = default;
    class DateProxy: public ObservableValue<Date>{
    public:
        DateProxy() : ObservableValue<Date>(Date()) {}  // note Date::today() not saved internally by default
        DateProxy& operator=(const Date& d) {
            if (value() != d)  // avoid notifications if the date doesn't actually change
                ObservableValue<Date>::operator=(d);
            return *this;
        }
        operator Date() const {
            if(value() == Date())
                return Date::today(); // default to today when converted to Date
            else
                return value();
        }
    };
    friend std::ostream& operator<<(std::ostream&, const DateProxy&);
public:
    //! the date at which pricing is to be performed.
    /*! Client code can inspect the evaluation date, as in:
        \code
        Date d = Settings::instance().evaluationDate();
        \endcode
        where today's date is returned if the evaluation date is
        set to the null date (its default value;) can set it to a
        new value, as in:
        \code
        Settings::instance().evaluationDate() = d;
        \endcode
        and can register with it, as in:
        \code
        registerWith(Settings::instance().evaluationDate());
        \endcode
        to be notified when it is set to a new value.
        \warning a notification is not sent when the evaluation
                 date changes for natural causes---i.e., a date
                 was not explicitly set (which results in today's
                 date being used for pricing) and the current date
                 changes as the clock strikes midnight.
    */
    DateProxy& evaluation_date() {return _evaluation_date;}
    const DateProxy& evaluation_date() const {return _evaluation_date;}
    
    
    /* ! Call this to prevent the evaluation date to change at
        midnight (and, incidentally, to gain quite a bit of
        performance.)  If no evaluation date was previously set,
        it is equivalent to setting the evaluation date to
        Date::todaysDate(); if an evaluation date other than
        Date() was already set, it has no effect.
    */
    void anchor_evaluation_date(){
        // set to today's date is not already set
        if(_evaluation_date.value() == Date())
            _evaluation_date = Date::today(); // will trigger notifications
        // if set, nothing since the date is already anchored
    }
     
    /* Call this to reset the evaluation date to
        Date::todaysDate() and allow it to change at midnight.  It
        is equivalent to setting the evaluation date to Date().
        This comes at the price of losing some performance, since
        the evaluation date is re-evaluated each time it is read.
    */
    void reset_evaluation_date() {_evaluation_date = Date();}

    /* This flag specifies whether or not Events occurring on the reference
        date should, by default, be taken into account as not happened yet.
        It can be overridden locally when calling the Event::hasOccurred
        method.
    */
    bool& include_reference_date_events() {return _include_reference_date_events;}
    bool include_reference_date_events() const {return _include_reference_date_events;}

    /* If set, this flag specifies whether or not CashFlows
        occurring on today's date should enter the NPV.  When the
        NPV date (i.e., the date at which the cash flows are
        discounted) equals today's date, this flag overrides the
        behavior chosen for includeReferenceDate. It cannot be overridden
        locally when calling the CashFlow::hasOccurred method.
    */
    boost::optional<bool>& include_todays_cashflows() {return _include_todays_cashflows;}
    boost::optional<bool> include_todays_cashflows() const {return _include_todays_cashflows;}
    
    bool& enforces_todays_historic_fixings() {return _enforces_todays_historic_fixings;}
    bool enforces_todays_historic_fixings() const {return _enforces_todays_historic_fixings;}


private:
    DateProxy _evaluation_date;
    bool _include_reference_date_events = false;
    boost::optional<bool> _include_todays_cashflows;
    bool _enforces_todays_historic_fixings = false;
};

// helper class to temporarily and safely change the settings
// capture settings
class SavedSettings {
public:
    SavedSettings()
    :_evaluation_date(Settings::instance().evaluation_date()),
    _include_reference_date_events(Settings::instance().include_reference_date_events()),
    _include_todays_cashflows(Settings::instance().include_todays_cashflows()),
    _enforces_todays_historic_fixings(Settings::instance().enforces_todays_historic_fixings()){}
    ~SavedSettings() {
        try{
            // reset to saved settings, when it is destroyed
            if(Settings::instance().evaluation_date() != _evaluation_date)
                Settings::instance().evaluation_date() = _evaluation_date; // trigger notifications
            Settings::instance().include_reference_date_events() = _include_reference_date_events;
            Settings::instance().include_todays_cashflows() = _include_todays_cashflows;
            Settings::instance().enforces_todays_historic_fixings() = _enforces_todays_historic_fixings;
        } catch (...){
            // nothing we can do except bailing out.
        }
    }
private:
    Date _evaluation_date;
    bool _include_reference_date_events = false;
    boost::optional<bool> _include_todays_cashflows;
    bool _enforces_todays_historic_fixings = false;
};



};




#endif /* settings_hpp */
