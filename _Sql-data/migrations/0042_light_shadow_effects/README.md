# Light/Shadow Effects Migration

This migration adds support for the new light/shadow effects system in SneezyMUD.

## Changes

1. Adds a `light_effect` column to the `obj` table
2. Initializes the light effect values based on existing GLOW and SHADOWY flags
3. Fixes paradoxical items (with both GLOW and SHADOWY flags)

## Implementation Details

The light effect value is calculated based on the object's volume:
- Glowing objects: positive value = MAX(1, FLOOR(volume / 10))
- Shadowy objects: negative value = -MAX(1, FLOOR(volume / 10))
- Normal objects: zero value

For objects that have both GLOW and SHADOWY flags (which is paradoxical), we prioritize the SHADOWY flag and remove the GLOW flag.

## Rollback

To roll back this migration, run the following SQL:

```sql
ALTER TABLE obj DROP COLUMN light_effect;
```

## Testing

After applying this migration, you should test the light/shadow effects system using the following commands:

1. `@stat <object>` - Check that the light effect value is displayed correctly
2. `@set light <object> <value>` - Set a positive light effect on an object
3. `@set shadow <object> <value>` - Set a negative light effect on an object
4. `@clear light <object>` - Clear any light effect from an object
