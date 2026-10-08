#ifndef HEALTHCARE_ANALYSIS_HPP
#define HEALTHCARE_ANALYSIS_HPP

// ============================================================================
// HealthcareAnalysis.hpp
// Compatibility wrapper delegating to expenditure_array.hpp and expenditure_list.hpp
// ============================================================================

#include "common.hpp"
#include "PatientArray.hpp"
#include "PatientList.hpp"
#include "expenditure_array.hpp"
#include "expenditure_list.hpp"

inline void healthcareExpenditureAnalysis_Array(PatientArray datasets[]) {
    expRunServiceAnalysisMenu(datasets);
}

inline void clinicalInsights_Array(PatientArray datasets[]) {
    displayClinicalInsightsReportArray(datasets);
    waitForEnter();
}

inline void healthcareExpenditureAnalysis_List(PatientList datasets[]) {
    lstRunServiceAnalysisMenu(datasets);
}

inline void clinicalInsights_List(PatientList datasets[]) {
    displayClinicalInsightsReportList(datasets);
    waitForEnter();
}

#endif // HEALTHCARE_ANALYSIS_HPP
