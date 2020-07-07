#ifndef _PAD_MODEL_H_
#define _PAD_MODEL_H_

#include "include.h"

class Pad
{
  int m_index;
  bool m_engaged = false;
  bool m_playing = false;

  public:
  Pad( int index )
    : m_index( index ),
    m_engaged( false ),
    m_playing( false )
  {}

  int index() const
  {
    return m_index;
  }

  bool engaged() const
  {
    return m_engaged;
  }

  bool playing() const
  {
    return m_playing;
  }

  void setEngaged( bool engaged )
  {
    m_engaged = engaged;
  }

  void setPlaying( bool playing )
  {
    m_playing = playing;
  }

  void reset()
  {
    m_engaged = false;
    m_playing = false;
  }

  void toggleEngaged()
  {
    m_engaged = !m_engaged;
  }
};

class PadModel : public QAbstractListModel
{
  Q_OBJECT

  Q_PROPERTY( int padSize READ padSize WRITE setPadSize NOTIFY padSizeChanged )

  /* Current dimensions of the grid */
  int PADSIZE = 16;

  /* Maximum supported dimensions of the grid */
  static constexpr int MAXPADSIZE = 16;

  /* List of Pad states */
  QList<Pad> m_pads;

  protected:

  /* For using `PadModel` as a model for `GridLayout` */
  QHash<int, QByteArray> roleNames() const
  {
    QHash<int, QByteArray> roles;

    roles[IndexRole] = "index";
    roles[EngagedRole] = "engaged";
    roles[PlayingRole] = "playing";

    return roles;
  }

  public:

  /* For using `PadModel` as a model for `GridLayout` */
  enum PadRoles {
    IndexRole = Qt::UserRole + 1,
    EngagedRole,
    PlayingRole
  };

  /* Constructur */
  PadModel( QObject *parent = 0 ) : QAbstractListModel( parent )
  {
    newGrid();
  }

  /* Add a pad to the pool */
  void addPad( const Pad &pad )
  {
    beginInsertRows( QModelIndex(), rowCount(), rowCount() );
    m_pads << pad;
    endInsertRows();
  }

  /* The number of pads */
  int rowCount( QModelIndex const& parent = QModelIndex() ) const
  {
    Q_UNUSED(parent);
    return m_pads.count();
  }

  /* For retrieving values */
  QVariant data( QModelIndex const& index, int role = Qt::DisplayRole ) const
  {
    if (index.row() < 0 || index.row() >= m_pads.count())
    {
      return QVariant();
    }

    Pad const& pad = m_pads[ index.row() ];

    if ( role == IndexRole )
    {
      return pad.index();
    }
    else if ( role == EngagedRole )
    {
      return pad.engaged();
    }
    else if ( role == PlayingRole )
    {
      return pad.playing();
    }

    return QVariant();
  }

  /* One tick */
  void update( int col )
  {
    /* Play the sounds ASAP */
    std::vector<int> note_idxs;
    for (int i = 0 ; i < PADSIZE ; i += 1)
    {
      if ( m_pads[ col + i * PADSIZE ].engaged() )
      {
        note_idxs.push_back( i );
      }
    }
    emit columnEngagedSignal( note_idxs );

    int prev_col = col == 0 ? PADSIZE - 1 : col - 1;
    for ( int i = 0 ; i < PADSIZE ; i += 1 )
    {
      /* Setup current column */
      {
        QModelIndex idx = createIndex( col + i * PADSIZE, 0 );
        Pad& pad = m_pads[ idx.row() ];

        pad.setPlaying( true );
        emit dataChanged( idx, idx, { PlayingRole } );
      }

      /* Clear previous column */
      {
        QModelIndex idx = createIndex( prev_col + i * PADSIZE, 0 );
        m_pads[ idx.row() ].setPlaying( false );
        emit dataChanged(idx, idx, { PlayingRole });
      }
    }
  }

  /* Get the grid size */
  int padSize()
  {
    return PADSIZE;
  }

  /* Set the grid size */
  void setPadSize( int size )
  {
    PADSIZE = size;

    emit padSizeChanged( PADSIZE );
  }

  public slots:

  /* Toggle a pad
   * index is into the 1D m_pads array */
  void toggleEngaged( int index )
  {
    m_pads[ index ].toggleEngaged();

    if ( m_pads[ index ].engaged() )
    {
      /* Send an index into the column */
      emit padEngaged( index / PADSIZE );
    }

    QModelIndex i = createIndex( index, 0 );
    emit dataChanged(i, i, { EngagedRole });
  }

  bool isEngaged( int index )
  {
    return m_pads[ index ].engaged();
  }

  /* Clear state of all pads */
  void clear()
  {
    for ( int i = 0 ; i < PADSIZE * PADSIZE ; i += 1 )
    {
      QModelIndex idx = createIndex( i, 0 );

      m_pads[ idx.row() ].reset();

      emit dataChanged(idx, idx, { EngagedRole, PlayingRole });
    }
  }

  /* Create a new grid */
  void newGrid()
  {
    beginRemoveRows( QModelIndex(), 0, rowCount() );
    m_pads.clear();
    endRemoveRows();

    for (int i = 0 ; i < (PADSIZE * PADSIZE) ; i++ )
    {
      addPad( Pad( i ) );
    }

    emit padSizeChanged( PADSIZE );
  }

  /* Randomize the engaged pads */
  void random( QString pattern )
  {
    clear();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib( 0, PADSIZE - 1 );

    for ( int i = 0 ; i < PADSIZE ; i += 1 )
    {
      int numToToggle = pattern[ i % pattern.count() ].digitValue();
      numToToggle = numToToggle > PADSIZE ? PADSIZE : numToToggle;

      std::vector<bool> isSet( PADSIZE, false );

      for ( int j = 0 ; j < numToToggle ; j += 1 )
      {
        int newInt = distrib( gen );
        while ( isSet[ newInt ] )
        {
          newInt = distrib( gen );
        }

        isSet[ newInt ] = true;

        toggleEngaged( i + newInt * PADSIZE );
      }
      QCoreApplication::processEvents();
    }
  }

  signals:

  void padSizeChanged( int size );
  void padEngaged( int index );
  void columnEngagedSignal( std::vector<int> notes );
};

#endif
