// Copyright (c) 2010 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

//
// C wrapper to libmetrics
//

#include "c_metrics_library.h"
#include "metrics_library.h"

extern "C" CMetricsLibrary CMetricsLibraryNew(void) {
  MetricsLibrary* lib = new MetricsLibrary;
  return reinterpret_cast<CMetricsLibrary>(lib);
}

extern "C" void CMetricsLibraryDelete(CMetricsLibrary handle) {
  MetricsLibrary* lib = reinterpret_cast<MetricsLibrary*>(handle);
  delete lib;
}

extern "C" void CMetricsLibraryInit(CMetricsLibrary handle) {
  MetricsLibrary* lib = reinterpret_cast<MetricsLibrary*>(handle);
  if (lib != NULL)
    lib->Init();
}

extern "C" int CMetricsLibrarySendToUMA(CMetricsLibrary handle,
					const char* name, int sample,
					int min, int max, int nbuckets) {
  MetricsLibrary* lib = reinterpret_cast<MetricsLibrary*>(handle);
  if (lib == NULL)
    return 0;
  return lib->SendToUMA(std::string(name), sample, min, max, nbuckets);
}

extern "C" int CMetricsLibrarySendEnumToUMA(CMetricsLibrary handle,
					    const char* name, int sample,
					    int max) {
  MetricsLibrary* lib = reinterpret_cast<MetricsLibrary*>(handle);
  if (lib == NULL)
    return 0;
  return lib->SendEnumToUMA(std::string(name), sample, max);
}
