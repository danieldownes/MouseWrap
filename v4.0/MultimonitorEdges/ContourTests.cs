namespace MultimonitorEdges;

using System.Collections.Generic;
using NUnit.Framework;

[TestFixture]
public class ContourTests
{
    [Test]
    public void GetContour_SingleRectangle()
    {
        var rectangles = new List<Rectangle>
        {
            new Rectangle(0, 10, 0, 30),
        };

        var contour = Contour.GetContour(rectangles);

        Assert.AreEqual(4, contour.Count);
    }

    [Test]
    public void GetContour_TwoRectangles_WithIntersection_CreatesHole()
    {
        var rectangles = new List<Rectangle>
        {
            new Rectangle(0, 10, 0, 30),
            new Rectangle(10, 15, 0, 40)
        };

        var contour = Contour.GetContour(rectangles);

        // Assert that contour has correct number of edges and creates a hole

        Assert.Contains(new Edge(0, 0, 0, 30, true), contour);
        Assert.Contains(new Edge(0, 10, 30, 30, false), contour);
        Assert.Contains(new Edge(10, 10, 30, 40, false), contour); // Nice hole
        Assert.Contains(new Edge(15, 15, 0, 40, false), contour);
        Assert.Contains(new Edge(10, 15, 0, 0, true), contour);
        Assert.Contains(new Edge(10, 15, 40, 40, false), contour);
    }
}