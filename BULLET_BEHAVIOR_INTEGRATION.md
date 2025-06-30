# Bullet Configuration Update - Behavior Integration

## Changes Made

### 1. Modified Behavior Configs
Updated three behavior configurations to use bullet IDs instead of full BulletConfig objects:

- **BehaviorShootBarrageConfig**
- **BehaviorShootProjectileConfig** 
- **BehaviorSpreadShotConfig**

#### Changes:
- `BulletConfig bulletConfig` → `std::string bulletId`
- Updated serialization to use bullet ID directly
- Updated deserialization to store bullet ID
- Updated GetConfigFields() to use new BulletID field type

### 2. Added New ConfigField Type
- Added `ConfigFieldType::BulletID` enum value
- Added constructor for BulletID field type in ConfigField struct
- This allows behaviors to have dropdown selection of available bullets

### 3. Updated UI Rendering
- Added case for `ConfigFieldType::BulletID` in UIMonsterProperties::RenderConfigFields()
- Renders as dropdown combo box showing available bullet IDs
- Auto-selects current bullet ID when editing behaviors

### 4. Bullet ID Management
- Added `GetAvailableBulletIds()` method to UIMonsterProperties
- Currently provides default bullet IDs (can be improved to access actual registry)
- Bullet selection updates behavior config immediately

## How It Works

### Before:
```cpp
struct BehaviorShootBarrageConfig {
    BulletConfig bulletConfig;  // Full bullet configuration object
    // ...
};
```

### After:
```cpp
struct BehaviorShootBarrageConfig {
    std::string bulletId;  // Reference to bullet by ID
    // ...
};
```

### UI Integration:
1. When editing monster behaviors, bullet fields now show as dropdowns
2. Dropdown lists available bullet IDs from the app
3. Selected bullet ID is stored in the behavior config
4. On export, the bullet ID is used directly in JSON

## Benefits

1. **Separation of Concerns**: Behaviors reference bullets, don't own full configs
2. **Data Consistency**: Single source of truth for bullet definitions
3. **UI Flexibility**: Easy to add/remove bullets without changing all behaviors
4. **Memory Efficiency**: Behaviors store references, not full objects
5. **Export Simplicity**: Direct bullet ID usage in JSON output

## Usage

1. **Create bullets** in Bullet Palette with unique names/IDs
2. **Edit monster behaviors** - bullet fields now show dropdowns
3. **Select bullets** from available options
4. **Export** - behavior JSON uses bullet IDs directly

## Next Steps

To fully complete the integration:

1. **Improve GetAvailableBulletIds()**: Connect to actual BulletRegistry
2. **Add validation**: Ensure selected bullet IDs exist  
3. **Add event handling**: Update UI when bullets are added/removed
4. **Enhance UX**: Show bullet names alongside IDs in dropdowns

The system now supports bullet ID-based behavior configuration with dropdown selection from available bullets in the application.
