-- Light/Shadow Effects Migration Script
-- This script adds a light_effect column to the obj table and initializes it based on existing flags

-- Add the light_effect column to the obj table
ALTER TABLE obj ADD COLUMN light_effect INT DEFAULT 0;

-- Update glowing objects to have a positive light effect
-- The light effect value is calculated based on the object's volume
UPDATE obj SET light_effect = GREATEST(1, FLOOR(volume / 10))
WHERE obj_flags & (1 << 1) > 0 AND NOT (obj_flags & (1 << 10) > 0);

-- Update shadowy objects to have a negative light effect
-- The light effect value is calculated based on the object's volume
UPDATE obj SET light_effect = -GREATEST(1, FLOOR(volume / 10))
WHERE obj_flags & (1 << 10) > 0 AND NOT (obj_flags & (1 << 1) > 0);

-- Fix paradoxical items (with both GLOW and SHADOWY flags)
-- We'll prioritize the SHADOWY flag and remove the GLOW flag
UPDATE obj SET obj_flags = obj_flags & ~(1 << 1), light_effect = -GREATEST(1, FLOOR(volume / 10))
WHERE obj_flags & (1 << 1) > 0 AND obj_flags & (1 << 10) > 0;

-- Log the number of affected objects
SELECT CONCAT('Updated ', COUNT(*), ' objects with light effects') AS migration_status FROM obj WHERE light_effect != 0;
