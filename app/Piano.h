#ifndef _APP_PIANO_H_
#define _APP_PIANO_H_

#include "include.h"

struct Piano : public QObject
{
  Q_OBJECT

  /* The maximum number of notes in the column */
  static constexpr int MAXCHORDSIZE = 16;

  /* Offset between array index and midi note definition */
  static constexpr int midinote_offset = 24;

  /* Cache of wav files */
  std::vector<Mix_Chunk*> m_wavs;

  std::vector<const char*> m_wav_filenames =
  {
    "wav/C1.wav",
    "wav/Db1.wav",
    "wav/D1.wav",
    "wav/Eb1.wav",
    "wav/E1.wav",
    "wav/F1.wav",
    "wav/Gb1.wav",
    "wav/G1.wav",
    "wav/Ab1.wav",
    "wav/A1.wav",
    "wav/Bb1.wav",
    "wav/B1.wav",

    "wav/C2.wav",
    "wav/Db2.wav",
    "wav/D2.wav",
    "wav/Eb2.wav",
    "wav/E2.wav",
    "wav/F2.wav",
    "wav/Gb2.wav",
    "wav/G2.wav",
    "wav/Ab2.wav",
    "wav/A2.wav",
    "wav/Bb2.wav",
    "wav/B2.wav",

    "wav/C3.wav",
    "wav/Db3.wav",
    "wav/D3.wav",
    "wav/Eb3.wav",
    "wav/E3.wav",
    "wav/F3.wav",
    "wav/Gb3.wav",
    "wav/G3.wav",
    "wav/Ab3.wav",
    "wav/A3.wav",
    "wav/Bb3.wav",
    "wav/B3.wav",

    "wav/C4.wav",
    "wav/Db4.wav",
    "wav/D4.wav",
    "wav/Eb4.wav",
    "wav/E4.wav",
    "wav/F4.wav",
    "wav/Gb4.wav",
    "wav/G4.wav",
    "wav/Ab4.wav",
    "wav/A4.wav",
    "wav/Bb4.wav",
    "wav/B4.wav",

    "wav/C5.wav",
    "wav/Db5.wav",
    "wav/D5.wav",
    "wav/Eb5.wav",
    "wav/E5.wav",
    "wav/F5.wav",
    "wav/Gb5.wav",
    "wav/G5.wav",
    "wav/Ab5.wav",
    "wav/A5.wav",
    "wav/Bb5.wav",
    "wav/B5.wav",
  };

  /* Maps QString -> int */
  QMap<QString, int> m_note_map =
  {
    { "C" , 0 },
    { "Db" , 1 },
    { "C#" , 1 },
    { "D" , 2 },
    { "Eb" , 3 },
    { "D#" , 3 },
    { "E" , 4 },
    { "F" , 5 },
    { "Gb" , 6 },
    { "F#" , 6 },
    { "G" , 7 },
    { "Ab" , 8 },
    { "G#" , 8 },
    { "A" , 9 },
    { "Bb" , 10 },
    { "A#" , 10 },
    { "B" , 11 }
  };

  /* Maps QString -> List of chord notes */
  QMap<QString, QList<int>> m_quality_map =
  {
    // C  Db D  Eb E  F  Gb G  Ab A  Bb B
    // 00 01 02 03 04 05 06 07 08 09 10 11
    // 12 13 14 15 16 17 18 19 20 21 22 23
    // 24 25 26 27 28 29 30 31 32 33 34 35
    // 36 37 38 39 40 41 42 43 44 45 46 47

    // 1 3 5 maj
    { "Maj7",     { 0, 4, 7, 11, 12, 16, 19, 23, 24, 28, 31, 35, 36, 40, 43, 47 } },
    // 1 3 5 maj 9
    { "Maj9",     { 0, 2, 4, 7,  11, 12, 14, 19, 23, 24, 26, 28, 31, 35, 36, 38 } },
    // 1 3 5 7
    { "7",        { 0, 4, 7, 10, 12, 16, 19, 22, 24, 28, 31, 34, 36, 40, 43, 46 } },
    // 1 3 7 9
    { "9",        { 0, 2, 4, 7,  10, 12, 14, 19, 22, 24, 26, 28, 31, 34, 36, 38 } },
    // 1 b3 5 7
    { "-7",       { 0, 3, 7, 10, 12, 15, 19, 22, 24, 27, 31, 34, 36, 40, 43, 46 } },
    // 1 3 #5 7
    { "+7",       { 0, 4, 8, 10, 12, 16, 20, 22, 24, 28, 32, 34, 36, 40, 44, 46 } },
    // 1 b3 b5 7
    { "half dim", { 0, 3, 6, 10, 12, 15, 18, 22, 24, 27, 30, 34, 36, 40, 42, 46 } },
    // 1 b3 b5 b7
    { "O",        { 0, 3, 6, 9,  12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 41, 44 } },
    // 1 b3 5 maj
    { "minMaj",   { 0, 3, 7, 11, 12, 15, 19, 23, 24, 27, 31, 35, 36, 39, 43, 47 } },
    // 1 3 #5 maj 1
    { "Maj7#5",   { 0, 4, 8, 11, 12, 16, 20, 23, 24, 28, 32, 35, 36, 40, 44, 47 } },
    // 1 3 b5 7 1
    { "7#11",     { 0, 4, 6,  7, 10, 12, 16, 18, 19, 22, 24, 28, 30, 31, 34, 36 } },
    // 1 3 5 7 #9
    { "7#9",      { 0, 3, 4, 7, 10, 12,  15, 16, 19, 22, 24, 27, 28, 31, 34, 36 } },
    // 1 3 #5 7 #9
    { "7#5#9",    { 0, 3, 4, 8, 10, 12,  15, 16, 20, 22, 24, 27, 28, 32, 34, 36 } },
    // 1 3 5 7 b9
    { "7b9",      { 0, 1, 4, 7, 10, 12,  13, 16, 19, 22, 24, 25, 28, 31, 34, 36 } },
    // 1 3 5 7 9
    { "-9",       { 0, 2, 3, 7, 10, 12, 14, 15, 19, 22, 24, 26, 27, 31, 34, 36 } },
    // 1 b3 5 7 b9
    { "-7b9",     { 0, 1, 3, 7, 10, 12, 13, 15, 19, 22, 24, 25, 27, 31, 34, 36 } },
    // 1 b3 5 1
    { "min",      { 0, 3, 5, 7,  12, 15, 17, 19, 24, 26, 27, 29, 31, 32, 34, 36 } }
  };

  /* Current root note */
  QString m_root_note = "C";

  /* Current chord quality */
  QString m_quality = "Maj7";

  /* Current chord notes */
  std::vector<int> m_chord;

  /* Load the note indexes of the chord */
  void loadChord()
  {
    m_chord.clear();
    m_chord.reserve( MAXCHORDSIZE );

    QList<int> quality = m_quality_map[ m_quality ];
    int root           = m_note_map[ m_root_note ];

    int a = 0;
    int b = MAXCHORDSIZE - 1;

    for ( auto& q : quality )
    {
      q = q % m_wavs.size();
    }

    std::sort( quality.begin(), quality.end() );

    for ( int i = b ; i >= a ; i -= 1 )
    {
      int q = quality[ i ] + root;
      m_chord.push_back( q % m_wavs.size() );
    }
  }

  public:

  /* Constructor */
  Piano()
  {
    // Initialize SDL
    if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
    {
      qDebug() << "[sdl init]" << SDL_GetError();
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
      qDebug() << "[mix open audio]" << Mix_GetError();
    }

    /* maximum volume on all channels */
    Mix_Volume( -1, 128 );
    /* set number of simultaneous wavs */
    Mix_AllocateChannels( MAXCHORDSIZE * MAXCHORDSIZE * MAXCHORDSIZE );

    m_wavs.reserve( m_wav_filenames.size() );

    /* Load wavs */
    for ( int i = 0 ; i < (int) m_wav_filenames.size() ; i++ )
    {
      auto m = Mix_LoadWAV( m_wav_filenames[ i ] );
      if ( ! m )
      {
        qDebug() << "[mix load wav] " << Mix_GetError()
          << m_wav_filenames[ i ];
      }

      m_wavs.push_back( m );
    }

    loadChord();
  }

  /* Destructor */
  ~Piano()
  {
    for ( Mix_Chunk* chunk_ptr : m_wavs )
    {
      Mix_FreeChunk( chunk_ptr );
    }

    Mix_Quit();
    SDL_Quit();
  }

  /* Play the note indices into m_chord */
  void play( std::vector<int> note_idxs )
  {
    /* Play the sounds ASAP */
    for (int i = 0 ; i < (int) note_idxs.size() ; i += 1)
    {
      if ( Mix_PlayChannel( -1, m_wavs[ m_chord[ note_idxs[ i ] ] ], 0 ) == -1 )
      {
        qDebug() << Mix_GetError();
      }
    }
  }

  /* Play a single note. `index` is into m_chord. */
  void play( int index )
  {
    if ( Mix_PlayChannel( -1, m_wavs[ m_chord[ index ] ], 0 ) == -1 )
    {
      qDebug() << Mix_GetError();
    }
  }

  /* Get the number of notes supported */
  int chordSize()
  {
    return MAXCHORDSIZE;
  }

  /* Get the current chord quality */
  QString quality()
  {
    return m_quality;
  }

  /* Set the current chord quality */
  void setQuality( QString quality )
  {
    m_quality = quality;
    loadChord();
  }

  /* Get the current root note */
  QString& rootNote()
  {
    return m_root_note;
  }

  /* Set the current root note */
  void setRootNote( QString root )
  {
    m_root_note = root;
    loadChord();
  }

  /* Return the midi note of the indexed note */
  int getMidiNote( int index )
  {
    return (m_wavs.size() - 1) - m_quality_map[ m_quality ][ index ] + m_note_map[ m_root_note ] + midinote_offset;
  }

  public slots:

  signals:

  void rootNoteChanged();
  void qualityChanged();
};

#endif
