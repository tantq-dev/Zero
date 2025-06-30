# DataHandler Updates for Bullet ID Structure

## Changes Made

### 1. Updated DataHandler Methods

#### GetBulletPropertiesFromMultipleBehavior()
- **Before**: Collected `BulletConfig` objects from behavior configs
- **After**: Collects `bulletId` strings from behavior configs using `std::unordered_set<std::string> m_bulletIds`

#### GetBulletsFromMonsters()
- **Before**: Only cleared `m_bullets` vector
- **After**: Clears both `m_bullets` and `m_bulletIds` collections

#### Export Logic (ExportAllToSingleJson)
- **Before**: Generated bullet JSON from collected `BulletConfig` objects
- **After**: 
  - Uses collected bullet IDs to lookup actual bullet definitions
  - Connects to `BulletRegistry` for real bullet data
  - Falls back to default values if bullet not found in registry
  - Logs warnings for missing bullet definitions

### 2. Added BulletRegistry Integration

#### DataHandler.h
- Added `#include <unordered_set>`
- Added `std::unordered_set<std::string> m_bulletIds` member
- Added `class BulletRegistry* m_bulletRegistry` member
- Added `SetBulletRegistry()` method

#### DataHandler.cpp
- Added `#include "BulletRegistry.h"`
- Updated export logic to use bullet registry when available
- Added bullet ID validation and logging

### 3. Connected UI to DataHandler

#### UIManager.h
- Added `GetBulletRegistry()` method to provide access to bullet registry

#### UIBulletPalette.h  
- Added `GetBulletRegistry()` method to expose internal registry

#### Tool.cpp
- Connected bullet registry from UIManager to DataHandler during initialization
- `m_dataHandler->SetBulletRegistry(m_uiManager->GetBulletRegistry())`

## How It Works Now

### Export Process:
1. **Collect Bullet IDs**: DataHandler scans behavior trees and collects all unique bullet IDs used
2. **Lookup Definitions**: For each bullet ID, looks up the actual bullet definition in the BulletRegistry
3. **Generate JSON**: Creates bullet JSON entries using real bullet data from registry
4. **Fallback**: If bullet not found in registry, uses reasonable defaults and logs warning

### Import Process:
- **Unchanged**: Import still works the same way, creating bullet configs map
- **Behavior Deserialization**: Now stores bullet IDs directly instead of looking up bullet configs

## Benefits

1. **Proper Data Flow**: Bullets created in UI are properly exported in JSON
2. **Data Consistency**: Export uses actual bullet definitions from the application
3. **Error Handling**: Missing bullets are handled gracefully with warnings
4. **Future-Proof**: System can be extended to handle more complex bullet data

## Error Handling

- If bullet registry is not set: Uses default bullet values
- If bullet ID not found in registry: Uses default values and logs warning  
- If bullet ID is empty: Skips the bullet entirely

## Example Flow

1. User creates bullets in Bullet Palette → stored in BulletRegistry
2. User assigns bullet IDs to monster behaviors → stored as strings in behavior configs
3. Export is triggered → DataHandler collects bullet IDs from behaviors
4. DataHandler looks up each bullet ID in BulletRegistry
5. Real bullet data is exported to JSON
6. Import reads bullet data and behavior bullet IDs correctly

The system now properly handles the transition from full BulletConfig objects to bullet ID references while maintaining compatibility with the existing export/import pipeline.
