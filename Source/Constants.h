/**
 * Constants.h:
 * 
 * This file defines a list of global constants that are used throughout PedalJUCE.
 * Each constant is accompanied by a short description of its purpose.
 */

#pragma once

/**
 * The maximum permissible distance in which a Connector connection can be made, in pixels.
 * Also used when painting Connector and InputPort objects.
 */
const int MAX_CONNECTION_RANGE = 10;

/**
 * A default sample rate value, in Hz.
 * This is frequently used as a dummy value in cases where we haven't detected the true sample rate
 * yet.
 */
const int DEFAULT_SAMPLE_RATE = 44100;

/**
 * A default block size, in samples.
 * This is frequently used as a dummy value in cases where we haven't detected the true block size
 * yet.
 */
const int DEFAULT_BLOCK_SIZE = 1024;

/**
 * The default width of the editor window, in pixels.
 */
const int DEFAULT_EDITOR_WIDTH = 1000;

/**
 * The default height of the editor window, in pixels.
 */
const int DEFAULT_EDITOR_HEIGHT = 750;