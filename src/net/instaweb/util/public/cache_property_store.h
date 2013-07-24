/*
 * Copyright 2013 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: pulkitg@google.com (Pulkit Goyal)
//
// Reads the properties stored in the CacheInterface and popluates them in
// PropertyPage.
// There is a CacheInterface object for every cohort which is stored in
// CohortCacheMap and read/write for a cohort happens on its respective
// CacheInterface object.

#ifndef NET_INSTAWEB_UTIL_PUBLIC_CACHE_PROPERTY_STORE_H_
#define NET_INSTAWEB_UTIL_PUBLIC_CACHE_PROPERTY_STORE_H_

#include <map>

#include "net/instaweb/http/public/user_agent_matcher.h"
#include "net/instaweb/util/public/property_cache.h"
#include "net/instaweb/util/public/property_store.h"
#include "pagespeed/kernel/base/basictypes.h"
#include "pagespeed/kernel/base/string_util.h"
#include "pagespeed/kernel/base/string.h"

namespace net_instaweb {

class CacheInterface;
class PropertyCacheValues;
class Statistics;
class ThreadSystem;
class Timer;

class CachePropertyStore : public PropertyStore {
 public:
  // Property cache key prefixes.
  static const char kPagePropertyCacheKeyPrefix[];

  // Does not take the ownership of cache, timer and stats object.
  // L2-only caches should be used for CachePropertyStore.  We cannot use the L1
  // cache because this data can get stale quickly.
  CachePropertyStore(const GoogleString& cache_key_prefix,
                     CacheInterface* cache,
                     Timer* timer,
                     Statistics* stats,
                     ThreadSystem* thread_system);
  virtual ~CachePropertyStore();

  // Cache lookup is initiated for the given cohort and results are populated
  // in PropertyPage if it is valid.
  virtual void Get(const GoogleString& url,
                   const GoogleString& options_signature_hash,
                   UserAgentMatcher::DeviceType device_type,
                   const PropertyCache::CohortVector& cohort_list,
                   PropertyPage* page,
                   BoolCallback* done);

  // Write to cache.
  virtual void Put(const GoogleString& url,
                   const GoogleString& options_signature_hash,
                   UserAgentMatcher::DeviceType device_type,
                   const PropertyCache::Cohort* cohort,
                   const PropertyCacheValues* values,
                   BoolCallback* done);

  // Establishes a Cohort backed by the CacheInteface passed to the constructor.
  void AddCohort(const GoogleString& cohort);
  // Establishes a Cohort to be backed by the specified CacheInterface.
  // Does not take the ownership of cache.
  void AddCohortWithCache(const GoogleString& cohort,
                          CacheInterface* cache);

  // Gets the underlying key associated with cache_key and a Cohort.
  // This is the key used for the CacheInterface provided to the
  // constructor.  This is made visible for testing, to make it
  // possible to inject delays into the cache via DelayCache::DelayKey.
  GoogleString CacheKey(const StringPiece& url,
                        const StringPiece& options_signature_hash,
                        UserAgentMatcher::DeviceType device_type,
                        const PropertyCache::Cohort* cohort) const;

  // Returns default cache backend associated with CachePropertyStore.
  const CacheInterface* cache_backend() { return default_cache_; }

 private:
  GoogleString cache_key_prefix_;
  typedef std::map<GoogleString, CacheInterface*> CohortCacheMap;
  CohortCacheMap cohort_cache_map_;
  CacheInterface* default_cache_;
  Timer* timer_;
  Statistics* stats_;
  ThreadSystem* thread_system_;
  DISALLOW_COPY_AND_ASSIGN(CachePropertyStore);
};

}  // namespace net_instaweb
#endif  // NET_INSTAWEB_UTIL_PUBLIC_CACHE_PROPERTY_STORE_H_
