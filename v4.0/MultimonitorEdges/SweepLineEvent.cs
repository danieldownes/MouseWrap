namespace MultimonitorEdges
{
    public class SweepLineEvent : IComparable<SweepLineEvent>
    {
        public int X { get; }
        public Edge Edge { get; }
        public bool IsStart { get; }

        public SweepLineEvent(int x, Edge edge, bool isStart)
        {
            X = x;
            Edge = edge;
            IsStart = isStart;
        }

        public int CompareTo(SweepLineEvent other)
        {
            if (X != other.X)
                return X.CompareTo(other.X);

            if (IsStart != other.IsStart)
                return IsStart ? -1 : 1;

            return 0;
        }
    }
}