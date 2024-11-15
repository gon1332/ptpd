/*-
 * Copyright (c) 2015-2024 Ioannis Konstantelias,
 * Copyright (c) 2012-2015 Wojciech Owczarek,
 * Copyright (c) 2011-2012 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen,
 *                         Inaqui Delgado,
 *                         Rick Ratzel,
 *                         National Instruments.
 * Copyright (c) 2009-2010 George V. Neville-Neil,
 *                         Steven Kreuzer,
 *                         Martin Burnicki,
 *                         Jan Breuer,
 *                         Gael Mace,
 *                         Alexandre Van Kempen
 * Copyright (c) 2005-2008 Kendall Correll, Aidan Williams
 *
 * All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef OUTLIERFILTER_H_
#define OUTLIERFILTER_H_

#include <dep/statistics.h>

#define OUTLIERFILTER_MAX_DESC 20

/**
 * @file   outlierfilter.h
 * @date   Fri Aug 22 16:18:33 2014
 *
 * @brief  Function definitions for the outlier filter
 *
 */

typedef struct {

    Boolean enabled;
    Boolean discard;
    Boolean autoTune;
    Boolean stepDelay;

    /* the user may have some conditions under which we should not filter.
       this is to hint them that we would like to always filter.
    */
    Boolean alwaysFilter;

    int capacity;
    double threshold;
    double weight;

    int minPercent;
    int maxPercent;
    double thresholdStep;

    double minThreshold;
    double maxThreshold;

    /* if absolute value outside this threshold, do not filter. negative = disabled */
    double maxAcceptable;

    /* accepted sample threshold at which step detection is active */
    int32_t stepThreshold;
    /* value which is considered a step */
    int32_t stepLevel;

    /* delay credit - gets used for waiting when step detected */
    int delayCredit;

    /* credit replenishment unit */
    int creditIncrement;

    /* safeguard - maximum credit */
    int maxDelay;

} OutlierFilterConfig;

typedef struct OutlierFilter OutlierFilter;

struct OutlierFilter {

    char id [OUTLIERFILTER_MAX_DESC + 1];
    OutlierFilterConfig config;

    DoubleMovingStdDev* rawStats;
    DoubleMovingMean* filteredStats;
    DoublePermanentMean outlierStats;
    DoublePermanentMean acceptedStats;
    Boolean lastOutlier;
    double threshold;
    double output;
    int autoTuneSamples;
    int autoTuneOutliers;
    int autoTuneScore;
    int consecutiveOutliers;
    int delay;
    int totalDelay;
    int delayCredit;
    Boolean blocking;

    /* 'methods' */

    int (*init)		(OutlierFilter *filter, OutlierFilterConfig *config, const char *id);
    int (*shutdown)	(OutlierFilter *filter);
    int (*reset)	(OutlierFilter *filter);
    Boolean (*filter)	(OutlierFilter *filter, double sample);
    Boolean (*configure) (OutlierFilter *filter, OutlierFilterConfig *config);
    int (*display)	(OutlierFilter *filter);
    void (*update)	(OutlierFilter *filter);

};



/* OutlierFilter *oFilterCreate(OutlierFilterConfig *options, const char *id); */
int outlierFilterSetup(OutlierFilter *filter);

#endif /*OUTLIERFILTER_H_*/
