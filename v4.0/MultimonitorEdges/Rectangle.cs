namespace MultimonitorEdges
{
    public class Rectangle
    {
        public int xMin, xMax, yMin, yMax;

        public Rectangle(int xMin, int xMax, int yMin, int yMax)
        {
            this.xMin = xMin;
            this.xMax = xMax;
            this.yMin = yMin;
            this.yMax = yMax;
        }

        public List<Edge> GetEdges()
        {
            return new List<Edge>
        {
            new Edge(xMin, xMin, yMin, yMax), // Left edge
            new Edge(xMax, xMax, yMin, yMax), // Right edge
            new Edge(xMin, xMax, yMin, yMin), // Bottom edge
            new Edge(xMin, xMax, yMax, yMax)  // Top edge
        };
        }
    }

}