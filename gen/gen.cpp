
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <climits>
#include <functional>
#include <algorithm>
#include <fstream>

const int LEVEL_WIDTH_MAX  = 500;
const int LEVEL_WIDTH_MIN  = 450;
const int LEVEL_HEGIHT_MAX = 80;
const int LEVEL_HEIGHT_MIN = 60;

const int STEP = 20;

const int PLAYER_JUMP_HEIGHT = 4;
const int PLAYER_JUMP_SIDEWAYS_OFFSET = 4;

enum GroundChars{ GROUND_CHAR_DEFAULT = 'G', GROUND_CHAR_SPEED_BONUS = 'S', GROUND_CHAR_JUMP_BONUS = 'J' };
const std::vector<char> GROUND_CHARS_VEC = { 'G', 'S', 'J' };
enum PlatformChars{ PLATFORM_CHAR_DEFAULT = 'p', PLATFORM_CHAR_SPEED_BONUS = 's', PLATFORM_CHAR_JUMP_BONUS = 'j' };
const std::vector<char> PLATFORM_CHARS_VEC = { 'p', 's', 'j' };
enum EnvironmentalEffects{ ENVO_EFFECT_CHANDELEER = 'c', ENVO_EFFECT_EMP = 'e' };
const char AIR_CHAR_DEFAULT = '-';
enum EntranceExitChars{ ENTRANCE_CHAR_DEFAULT = 'E', EXIT_CHAR_DEFAULT = 'X' };

const int GROUND_HEIGHT_MIN  = 1;
const int GROUND_HEIGHT_MAX = 25;

const int PLATFORM_WIDTH_MAX  = 20;
const int PLATFORM_WIDTH_MIN  = 8;

const int PLATFORMS_PER_STEP_MIN = 0;
const int PLATFORMS_PER_STEP_MAX = 13;

const int PLATFORM_GROUND_OFFSET = 2;
const int PLATFORM_PLATFORM_OFFSET = 1;

enum Bonuses{ BONUSES_BEGIN, SPEED_BONUS, JUMP_BONUS, BONUSES_TOTAL };
const int BONUS_OFFSET = 0;

void printField( std::ostream& stream, const std::vector<std::string>& Field, const int LEVEL_WIDTH, const int LEVEL_HEIGHT );
bool doesCollide( const std::vector<std::string>& Field, int platformPos, int platformWidth, int platformHeight );
bool platformIsReachable( const std::vector<std::string>& Field, int groundHeight, int platformPos, int platformWidth, int platformHeight );
bool groundIsReachable( int groundHeight, int groundHeightPrev );
void placeGround( std::vector<std::string>& Field, int position, int height, char fillChar );
void placePlatform( std::vector<std::string>& Field, int position, int width, int height, char fillChar );
bool rowHasSpace( const std::vector<std::string>& Field, int platformPos, int platformWidth, int platformHeight );
void finishLevel( std::vector<std::string>& Field, const int LEVEL_WIDTH, const int LEVEL_HEIGHT );



int main()
{
    int generatorSeed;
    std::cin >> generatorSeed;
    std::default_random_engine seedGen( generatorSeed );
    std::uniform_int_distribution<int> seedGenDist( INT_MIN, INT_MAX );

    auto seedGenerator = std::bind( seedGenDist, seedGen );

    std::default_random_engine levelParamGen( seedGenerator() );
    std::uniform_int_distribution<int> levelHeightDist( LEVEL_HEIGHT_MIN, LEVEL_HEGIHT_MAX );
    std::uniform_int_distribution<int> levelWidthDist( LEVEL_WIDTH_MIN, LEVEL_WIDTH_MAX );

    const int LEVEL_WIDTH  = levelWidthDist( levelParamGen );
    const int LEVEL_HEIGHT = levelHeightDist( levelParamGen );
    const int PLATFORM_HEIGHT_MAX = LEVEL_HEIGHT - 10;

    std::default_random_engine groundHeightGen ( seedGenerator() );
    std::uniform_int_distribution<int> groundHeightDist ( GROUND_HEIGHT_MIN, GROUND_HEIGHT_MAX );
    auto groundHeightGenerator = std::bind( groundHeightDist, groundHeightGen );

    std::default_random_engine platformsPerStepGen( seedGenerator() );
    std::uniform_int_distribution<int> platformsPerStepDist( PLATFORMS_PER_STEP_MIN, PLATFORMS_PER_STEP_MAX );
    auto platformsPerStepGenerator = std::bind( platformsPerStepDist, platformsPerStepGen );

    std::default_random_engine platformWidthGen ( seedGenerator() );
    std::uniform_int_distribution<int> platformWidthDist( PLATFORM_WIDTH_MIN, PLATFORM_WIDTH_MAX );
    auto platformWidthGenerator = std::bind( platformWidthDist, platformWidthGen );

    std::default_random_engine bonusGen ( seedGenerator() );
    std::uniform_int_distribution<int> bonusDist( BONUSES_BEGIN, BONUSES_TOTAL + BONUS_OFFSET );
    auto bonusGenerator = std::bind( bonusDist, bonusGen );

    std::vector<std::string> Field( LEVEL_HEIGHT, std::string( LEVEL_WIDTH, AIR_CHAR_DEFAULT ) );
    Field[0] = std::string( LEVEL_WIDTH, GROUND_CHAR_DEFAULT );
    int currentStep = STEP;

    int groundHeightPrev = 0;
    while( currentStep < LEVEL_WIDTH - STEP )
    {
        int groundHeight;
        do
        {
            groundHeight = groundHeightGenerator();
        }while( !groundIsReachable( groundHeight, groundHeightPrev ) );

        char groundChar;

        int groundBonus = bonusGenerator();
        switch( groundBonus )
        {
            case SPEED_BONUS:
                groundChar = GROUND_CHAR_SPEED_BONUS;
                break;
            case JUMP_BONUS:
                groundChar = GROUND_CHAR_JUMP_BONUS;
                break;
            default:
                groundChar = GROUND_CHAR_DEFAULT;
                break;
        }

        placeGround( Field, currentStep, groundHeight, groundChar );

        int platformsThisStep = platformsPerStepGenerator();

        while( platformsThisStep > 0 )
        {
            int platformWidth = platformWidthGenerator();

            int platformPos;
            std::default_random_engine platformPosGen( seedGenerator() );
            std::uniform_int_distribution<int> platformPosDist( currentStep, currentStep + STEP - 1 );
            platformPos = platformPosDist( platformPosGen );

            int platformHeight;
            std::default_random_engine platformHeightGen ( seedGenerator() );
            std::uniform_int_distribution<int> platformHeightDist ( groundHeight + PLATFORM_GROUND_OFFSET, PLATFORM_HEIGHT_MAX );
            do
            {
                platformHeight = platformHeightDist( platformHeightGen );

            }while( !platformIsReachable( Field, groundHeight, platformPos, platformWidth, platformHeight) && !rowHasSpace( Field, platformPos, platformWidth, platformHeight) );

        //    std::cout << 'c';
            char platformChar;

            int platformBonus = bonusGenerator();
            switch( platformBonus )
            {
                case SPEED_BONUS:
                    platformChar = PLATFORM_CHAR_SPEED_BONUS;
                    break;
                case JUMP_BONUS:
                    platformChar = PLATFORM_CHAR_JUMP_BONUS;
                    break;
                default:
                    platformChar = PLATFORM_CHAR_DEFAULT;
                    break;
            }

            placePlatform( Field, platformPos, platformWidth, platformHeight, platformChar );

            platformsThisStep--;
//            br++;
      //      std::cout << br;
        }

        currentStep += STEP;
    }

    finishLevel( Field, LEVEL_WIDTH, LEVEL_HEIGHT );

    printField( std::cout, Field, LEVEL_WIDTH, LEVEL_HEIGHT );

    /*for(int i = Field.size() - 1; i >=0; i--)
    {
        std::cout << Field[i] << std::endl;
    }*/

    return 0;
}

bool groundIsReachable( int groundHeight, int groundHeightPrev )
{
    if ( groundHeight - groundHeightPrev <= PLAYER_JUMP_HEIGHT )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool platformIsReachable( const std::vector<std::string>& Field, int groundHeight, int platformPos, int platformWidth, int platformHeight )
{
    bool isReach = false;

    for( int i = platformHeight; i > platformHeight - PLAYER_JUMP_HEIGHT && i > 0; i-- )
    {
        for( int j = platformPos  /*- PLAYER_JUMP_SIDEWAYS_OFFSET*/; j < platformPos + platformWidth /* + PLAYER_JUMP_SIDEWAYS_OFFSET*/ ; j++ )
        {
            if( Field[i][j] != AIR_CHAR_DEFAULT )
            {
                isReach = true;
                break;
            }
        }
    }

    return isReach;
}

bool doesCollide( const std::vector<std::string>& Field, int platformPos, int platformWidth, int platformHeight )
{
    bool doesCol = false;

    for( int j = platformPos - PLATFORM_PLATFORM_OFFSET; j <= platformPos + platformWidth + PLATFORM_PLATFORM_OFFSET; j++ )
    {
        if( Field[platformHeight][j] != AIR_CHAR_DEFAULT )
        {
            doesCol = true;
            break;
        }
        //std::cout << 'b';
    }

    return doesCol;
}

void printField( std::ostream& stream, const std::vector<std::string>& Field, const int LEVEL_WIDTH, const int LEVEL_HEIGHT )
{
    stream << LEVEL_HEIGHT << ' ' << LEVEL_WIDTH << std::endl;

    for( std::vector<std::string>::size_type it = Field.size() ; it--;)
    {
        stream << Field[it] << std::endl;
    }
}

void placeGround( std::vector<std::string>& Field, int position, int height, char fillChar )
{
    if ( fillChar == GROUND_CHAR_DEFAULT )
    {
       for( int i = 1; i <= height; i++ )
        {
            for( int j = position; j < position + STEP; j++ )
            {
                Field[i][j] = fillChar;
            }
        }
    }
    else
    {
        for( int i = 1; i <= height - 1; i++ )
        {
            for( int j = position; j < position + STEP; j++ )
            {
                Field[i][j] = GROUND_CHAR_DEFAULT;
            }
        }
        for( int j = position; j < position + STEP; j++ )
        {
            Field[height][j] = fillChar;
        }
    }
}

void placePlatform( std::vector<std::string>& Field, int position, int width, int height, char fillChar )
{
    for( int j = position; j < position + width; j++ )
    {
        Field[height][j] = fillChar;
    }
}

bool rowHasSpace( const std::vector<std::string>& Field, int platformPos, int platformWidth, int platformHeight )
{
    bool hasSpace = true;

    for(int i = platformPos - PLATFORM_PLATFORM_OFFSET; i < platformPos + platformWidth; i++ )
    {
        if( Field[platformHeight][i] != AIR_CHAR_DEFAULT )
        {
            hasSpace = false;
            break;
        }
    }

    return hasSpace;
}

void finishLevel( std::vector<std::string>& Field, const int LEVEL_WIDTH, const int LEVEL_HEIGHT )
{
    for(int i = 0; i < LEVEL_HEIGHT; i++)
    {
        Field[i][0] = GROUND_CHAR_DEFAULT;
        Field[i][LEVEL_WIDTH - 1] = GROUND_CHAR_DEFAULT;
    }

    for(int i = 0;i < LEVEL_WIDTH - 1; i++)
    {
        Field[LEVEL_HEIGHT-1][i] = GROUND_CHAR_DEFAULT;
    }

   /* for(int i = 1; i < 5; i++)
    {
        for(int j = 4; j < 8; j++)
        {
            Field[i][j] = ENTRANCE_CHAR_DEFAULT;
            Field[i][LEVEL_WIDTH - j - 1] = EXIT_CHAR_DEFAULT;
        }
    }*/
}


