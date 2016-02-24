/* 
 * File:   SeriesSampleExtended.h
 * Author: volchnik
 *
 * Created on August 3, 2014, 10:49 PM
 */

#ifndef SERIESSAMPLEEXTENDED_H
#define	SERIESSAMPLEEXTENDED_H

class SeriesSampleExtended {
public:
    SeriesSampleExtended();
    virtual ~SeriesSampleExtended();
    
    long datetime;
    float value;
    
    bool operator < (const SeriesSampleExtended& sample) const
    {
        return (this->datetime < sample.datetime);
    }
private:

};

#endif	/* SERIESSAMPLEEXTENDED_H */

